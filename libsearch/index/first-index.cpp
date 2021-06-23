/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
//#include <QtConcurrent>
#include "first-index.h"
#include <QDebug>

#define NEW_QUEUE(a) a = new QQueue<QString>(); qDebug("---------------------------%s %s %s new at %d..",__FILE__,__FUNCTION__,#a,__LINE__);
//#define DELETE_QUEUE(a )
using namespace Zeeker;
FirstIndex::FirstIndex() {
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(100);
}

FirstIndex::~FirstIndex() {
    qDebug() << "~FirstIndex";
    if(this->q_index)
        delete this->q_index;
    this->q_index = nullptr;
    if(this->q_content_index)
        delete this->q_content_index;
    this->q_content_index = nullptr;
    if(this->p_indexGenerator)
        delete this->p_indexGenerator;
    this->p_indexGenerator = nullptr;
    qDebug() << "~FirstIndex end";
}

void FirstIndex::DoSomething(const QFileInfo& fileInfo) {
//    qDebug() << "there are some shit here"<<fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0");
    this->q_index->enqueue(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString((fileInfo.isDir() && (!fileInfo.isSymLink())) ? "1" : "0"));
    if((fileInfo.fileName().split(".", QString::SkipEmptyParts).length() > 1) && (true == targetFileTypeMap[fileInfo.fileName().split(".").last()])) {
        //this->q_content_index->enqueue(fileInfo.absoluteFilePath());
        if (fileInfo.fileName().split(".").last() == "docx") {
            QuaZip file(fileInfo.absoluteFilePath());
            if(!file.open(QuaZip::mdUnzip))
                return;
            if(!file.setCurrentFile("word/document.xml", QuaZip::csSensitive))
                return;
            QuaZipFile fileR(&file);
            this->q_content_index->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileR.usize()));//docx解压缩后的xml文件为实际需要解析文件大小
            file.close();
        } else if (fileInfo.fileName().split(".").last() == "pptx") {
            QuaZip file(fileInfo.absoluteFilePath());
            if(!file.open(QuaZip::mdUnzip))
                return;
            QString prefix("ppt/slides/slide");
            qint64 fileSize(0);
            qint64 fileIndex(0);
            for(QString i : file.getFileNameList()) {
                if(i.startsWith(prefix)){
                    QString name = prefix + QString::number(fileIndex + 1) + ".xml";
                    fileIndex++;
                    if(!file.setCurrentFile(name)) {
                        continue;
                    }
                    QuaZipFile fileR(&file);
                    fileSize += fileR.usize();
                }
            }
            file.close();
            this->q_content_index->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileSize));//pptx解压缩后的xml文件为实际需要解析文件大小
        } else if (fileInfo.fileName().split(".").last() == "xlsx") {
            QuaZip file(fileInfo.absoluteFilePath());
            if(!file.open(QuaZip::mdUnzip))
                return;
            if(!file.setCurrentFile("xl/sharedStrings.xml", QuaZip::csSensitive))
                return;
            QuaZipFile fileR(&file);
            this->q_content_index->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileR.usize()));//xlsx解压缩后的xml文件为实际解析文件大小
            file.close();
        } else {
            this->q_content_index->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileInfo.size()));
        }
    }
}

void FirstIndex::run() {
    QTime t1 = QTime::currentTime();

    // Create a fifo at ~/.config/org.ukui/ukui-search, the fifo is used to control the order of child processes' running.
    QDir fifoDir = QDir(QDir::homePath() + "/.config/org.ukui/ukui-search");
    if(!fifoDir.exists())
        qDebug() << "create fifo path" << fifoDir.mkpath(fifoDir.absolutePath());

    unlink(UKUI_SEARCH_PIPE_PATH);
    int retval = mkfifo(UKUI_SEARCH_PIPE_PATH, 0777);
    if(retval == -1) {
        qCritical() << "creat fifo error!!";
        syslog(LOG_ERR, "creat fifo error!!\n");
        assert(false);
        return;
    }
    qDebug() << "create fifo success\n";

    QString indexDataBaseStatus =  IndexStatusRecorder::getInstance()->getStatus(INDEX_DATABASE_STATE).toString();
    QString contentIndexDataBaseStatus = IndexStatusRecorder::getInstance()->getStatus(CONTENT_INDEX_DATABASE_STATE).toString();
    QString inotifyIndexStatus = IndexStatusRecorder::getInstance()->getStatus(INOTIFY_NORMAL_EXIT).toString();

    qDebug() << "indexDataBaseStatus: " << indexDataBaseStatus;
    qDebug() << "contentIndexDataBaseStatus: " << contentIndexDataBaseStatus;
    qDebug() << "inotifyIndexStatus: " << inotifyIndexStatus;

    /* || contentIndexDataBaseStatus == ""*/
    if(indexDataBaseStatus == "") {
        this->bool_dataBaseExist = false;
    } else {
        this->bool_dataBaseExist = true;
    }
    if(indexDataBaseStatus != "2" || contentIndexDataBaseStatus != "2" || inotifyIndexStatus != "2") {
        this->bool_dataBaseStatusOK = false;
    } else {
        this->bool_dataBaseStatusOK = true;
    }

    this->q_index = new QQueue<QVector<QString>>();
    //this->q_content_index = new QQueue<QString>();
    //NEW_QUEUE(this->q_content_index);
//    this->mlm = new MessageListManager();
    this->q_content_index = new QQueue<QPair<QString,qint64>>();

    int fifo_fd;
    char buffer[2];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x1;
    buffer[1] = '\0';
    fifo_fd = open(UKUI_SEARCH_PIPE_PATH, O_RDWR);
    if(fifo_fd == -1) {
        perror("open fifo error\n");
        assert(false);
    }

//    this->q_content_index->enqueue(QString("/home/zhangzihao/Desktop/qwerty/四库全书.txt"));

//    this->p_indexGenerator->creatAllIndex(this->q_content_index);


    ++FileUtils::_index_status;
    pid_t pid;
    pid = fork();
    if(pid  == 0) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        prctl(PR_SET_NAME, "first-index");
        if(this->bool_dataBaseExist) {
            if(this->bool_dataBaseStatusOK) {
                ::_exit(0);
            } else {
                //if the parameter is false, index won't be rebuild
                //if it is true, index will be rebuild
                p_indexGenerator = IndexGenerator::getInstance(true, this);
            }
        } else {
//            p_indexGenerator = IndexGenerator::getInstance(false,this);
            p_indexGenerator = IndexGenerator::getInstance(true, this);

        }
        //TODO Fix these weird code.
        QSemaphore sem(5);
        QMutex mutex1, mutex2, mutex3;
        mutex1.lock();
        mutex2.lock();
        mutex3.lock();
        sem.acquire(4);
//        QtConcurrent::run([&](){
        sem.acquire(1);
        mutex1.unlock();
        this->setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        this->Traverse();
        FileUtils::_max_index_count = this->q_index->length();
        qDebug() << "max_index_count:" << FileUtils::_max_index_count;
        sem.release(5);
//        });
        QtConcurrent::run(&m_pool, [&]() {
            sem.acquire(2);
            mutex2.unlock();
            qDebug() << "index start;";
            QQueue<QVector<QString>>* tmp1 = new QQueue<QVector<QString>>();
            while(!this->q_index->empty()) {
                for(size_t i = 0; (i < 8192) && (!this->q_index->empty()); ++i) {
                    tmp1->enqueue(this->q_index->dequeue());
                }
                this->p_indexGenerator->creatAllIndex(tmp1);
                tmp1->clear();
            }
            delete tmp1;
            qDebug() << "index end;";
            sem.release(2);
        });
        QtConcurrent::run(&m_pool,[&]() {
            sem.acquire(2);
            mutex3.unlock();
            QQueue<QString>* tmp2 = new QQueue<QString>();
            qDebug() << "q_content_index:" << q_content_index->size();
            while(!this->q_content_index->empty()) {
                //                for (size_t i = 0; (i < this->u_send_length) && (!this->q_content_index->empty()); ++i){
                qint64 fileSize = 0;
                //修改一次处理的数据量，从30个文件改为文件总大小为50M以下，50M为暂定值--jxx20210519
                for(size_t i = 0;/* (i < 30) && (fileSize < 52428800) && */(!this->q_content_index->empty()); ++i) {
                    QPair<QString,qint64> tempPair = this->q_content_index->dequeue();
                    fileSize += tempPair.second;
                    if (fileSize > 52428800 ) {
                        if (tmp2->size() == 0) {
                            tmp2->enqueue(tempPair.first);
                            break;
                        }
                        this->q_content_index->enqueue(tempPair);
                        break;
                    }
                    tmp2->enqueue(tempPair.first);
                }
                //                qDebug() << ">>>>>>>>all fileSize:" << fileSize << "file num:" << tmp->size() << "<<<<<<<<<<<<<<<<<<<";
                this->p_indexGenerator->creatAllIndex(tmp2);
                tmp2->clear();
            }
            delete tmp2;
            qDebug() << "content index end;";
            sem.release(2);
        });

        mutex1.lock();
        mutex2.lock();
        mutex3.lock();
        sem.acquire(5);
        mutex1.unlock();
        mutex2.unlock();
        mutex3.unlock();

        if(this->q_index)
            delete this->q_index;
        this->q_index = nullptr;
        if(this->q_content_index)
            delete this->q_content_index;
        this->q_content_index = nullptr;
        if(p_indexGenerator)
            delete p_indexGenerator;
        p_indexGenerator = nullptr;
//        GlobalSettings::getInstance()->forceSync();
        IndexStatusRecorder::getInstance()->setStatus(INDEX_DATABASE_STATE, "2");
        IndexStatusRecorder::getInstance()->setStatus(CONTENT_INDEX_DATABASE_STATE, "2");
        ::_exit(0);
    } else if(pid < 0) {
        qWarning() << "First Index fork error!!";
    } else {
        waitpid(pid, NULL, 0);
        --FileUtils::_index_status;
    }

    IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "2");
    int retval1 = write(fifo_fd, buffer, strlen(buffer));
    if(retval1 == -1) {
        qWarning("write error\n");
    }
    qDebug("write data ok!\n");
    QTime t2 = QTime::currentTime();
    qWarning() << t1;
    qWarning() << t2;

    return;

}
