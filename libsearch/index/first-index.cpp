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
#include "dir-watcher.h"
#include <QDebug>

using namespace UkuiSearch;
FirstIndex *FirstIndex::m_instance = nullptr;
std::once_flag g_firstIndexInstanceFlag;
FirstIndex::FirstIndex() : m_semaphore(INDEX_SEM, 1, QSystemSemaphore::AccessMode::Open)
{
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(100);
}

FirstIndex *FirstIndex::getInstance()
{
    std::call_once(g_firstIndexInstanceFlag, [] () {
        m_instance = new FirstIndex;
    });
    return m_instance;
}

FirstIndex::~FirstIndex() {
    qDebug() << "~FirstIndex";
    if(this->m_indexData)
        delete this->m_indexData;
    this->m_indexData = nullptr;
    if(this->m_contentIndexData)
        delete this->m_contentIndexData;
    this->m_contentIndexData = nullptr;
    if(this->m_ocrIndexData)
        delete this->m_ocrIndexData;
    this->m_ocrIndexData = nullptr;
    qDebug() << "~FirstIndex end";
}

void FirstIndex::DoSomething(const QFileInfo& fileInfo) {
    //    qDebug() << "there are some shit here"<<fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0");
    this->m_indexData->enqueue(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString((fileInfo.isDir() && (!fileInfo.isSymLink())) ? "1" : "0"));
    if (fileInfo.fileName().split(".", QString::SkipEmptyParts).length() < 2)
        return;
    if (true == targetFileTypeMap[fileInfo.fileName().split(".").last()]
            and false == FileUtils::isEncrypedOrUnreadable(fileInfo.absoluteFilePath())) {
        if (fileInfo.fileName().split(".").last() == "docx") {
            QuaZip file(fileInfo.absoluteFilePath());
            if(!file.open(QuaZip::mdUnzip))
                return;
            if(!file.setCurrentFile("word/document.xml", QuaZip::csSensitive))
                return;
            QuaZipFile fileR(&file);
            this->m_contentIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileR.usize()));//docx解压缩后的xml文件为实际需要解析文件大小
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
            this->m_contentIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileSize));//pptx解压缩后的xml文件为实际需要解析文件大小
        } else if (fileInfo.fileName().split(".").last() == "xlsx") {
            QuaZip file(fileInfo.absoluteFilePath());
            if(!file.open(QuaZip::mdUnzip))
                return;
            if(!file.setCurrentFile("xl/sharedStrings.xml", QuaZip::csSensitive))
                return;
            QuaZipFile fileR(&file);
            this->m_contentIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileR.usize()));//xlsx解压缩后的xml文件为实际解析文件大小
            file.close();
        } else {
            this->m_contentIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileInfo.size()));
        }
    } else if (true == targetPhotographTypeMap[fileInfo.fileName().split(".").last()]) {
        if (FileUtils::isOcrSupportSize(fileInfo.absoluteFilePath())) {
            this->m_contentIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileInfo.size()));
            //this->m_ocrIndexData->enqueue(qMakePair(fileInfo.absoluteFilePath(),fileInfo.size()));
        }
    }
}

void FirstIndex::run() {
    QTime t1 = QTime::currentTime();
    // Create a fifo at ~/.config/org.ukui/ukui-search, the fifo is used to control the order of child processes' running.
    QString indexDataBaseStatus =  IndexStatusRecorder::getInstance()->getStatus(INDEX_DATABASE_STATE).toString();
    QString contentIndexDataBaseStatus = IndexStatusRecorder::getInstance()->getStatus(CONTENT_INDEX_DATABASE_STATE).toString();
//    QString ocrIndexDatabaseStatus = IndexStatusRecorder::getInstance()->getStatus(OCR_DATABASE_STATE).toString();
    QString inotifyIndexStatus = IndexStatusRecorder::getInstance()->getStatus(INOTIFY_NORMAL_EXIT).toString();

    qInfo() << "indexDataBaseStatus: " << indexDataBaseStatus;
    qInfo() << "contentIndexDataBaseStatus: " << contentIndexDataBaseStatus;
//    qInfo() << "ocrIndexDatabaseStatus: " << ocrIndexDatabaseStatus;
    qInfo() << "inotifyIndexStatus: " << inotifyIndexStatus;

    m_allDatadaseStatus = inotifyIndexStatus == "2" ? true : false;
    m_indexDatabaseStatus = indexDataBaseStatus == "2" ? true : false;
    m_contentIndexDatabaseStatus = contentIndexDataBaseStatus == "2" ? true : false;
//    m_ocrIndexDatabaseStatus = ocrIndexDatabaseStatus == "2" ? true : false;

    if(m_allDatadaseStatus && m_indexDatabaseStatus && m_contentIndexDatabaseStatus /*&& m_ocrIndexDatabaseStatus*/) {
        m_semaphore.release(1);
        return;
    }


    this->m_indexData = new QQueue<QVector<QString>>();
    this->m_contentIndexData = new QQueue<QPair<QString,qint64>>();
//    this->m_ocrIndexData = new QQueue<QPair<QString,qint64>>();

    ++FileUtils::indexStatus;
    pid_t pid;
    pid = fork();
    if(pid  == 0) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        prctl(PR_SET_NAME, "first-index");

        QSemaphore sem(5);
        QMutex mutex1, mutex2, mutex3;
        mutex1.lock();
        mutex2.lock();
        //        mutex3.lock();

        qInfo() << "index dir" << DirWatcher::getDirWatcher()->currentindexableDir();
        qInfo() << "index block dir" << DirWatcher::getDirWatcher()->currentBlackListOfIndex();
        setPath(DirWatcher::getDirWatcher()->currentindexableDir());
        setBlockPath(DirWatcher::getDirWatcher()->currentBlackListOfIndex());
        this->Traverse();

        FileUtils::maxIndexCount = this->m_indexData->length();
        qDebug() << "max_index_count:" << FileUtils::maxIndexCount;
        QtConcurrent::run(&m_pool, [&]() {
            sem.acquire(2);
            mutex1.unlock();
            if(m_allDatadaseStatus && m_indexDatabaseStatus) {
                sem.release(2);
                return;
            }
            qDebug() << "index start;" << m_indexData->size();
            IndexGenerator::getInstance()->rebuildIndexDatabase();
            QQueue<QVector<QString>>* tmp1 = new QQueue<QVector<QString>>();
            bool sucess = true;
            while(!this->m_indexData->empty()) {
                for(size_t i = 0; (i < 8192) && (!this->m_indexData->empty()); ++i) {
                    tmp1->enqueue(this->m_indexData->dequeue());
                }
                if(!IndexGenerator::getInstance()->creatAllIndex(tmp1)) {
                    sucess = false;
                    break;
                }
                tmp1->clear();
            }
            delete tmp1;
            qDebug() << "index end;";
            if(sucess) {
                IndexStatusRecorder::getInstance()->setStatus(INDEX_DATABASE_STATE, "2");
            }
            sem.release(2);
        });
        QtConcurrent::run(&m_pool,[&]() {
            sem.acquire(2);
            mutex2.unlock();
            if(m_allDatadaseStatus && m_contentIndexDatabaseStatus) {
                sem.release(2);
                return;
            }
            qDebug() << "content index start:" << m_contentIndexData->size();
            IndexGenerator::getInstance()->rebuildContentIndexDatabase();
            QQueue<QString>* tmp2 = new QQueue<QString>();
            bool sucess = true;
            while(!this->m_contentIndexData->empty()) {
                qint64 fileSize = 0;
                //修改一次处理的数据量，从30个文件改为文件总大小为50M以下，50M为暂定值--jxx20210519
                for(size_t i = 0;/* (i < 30) && (fileSize < 52428800) && */(!this->m_contentIndexData->empty()); ++i) {
                    QPair<QString,qint64> tempPair = this->m_contentIndexData->dequeue();
                    fileSize += tempPair.second;
                    if (fileSize > 52428800 ) {
                        if (tmp2->size() == 0) {
                            tmp2->enqueue(tempPair.first);
                            break;
                        }
                        this->m_contentIndexData->enqueue(tempPair);
                        break;
                    }
                    tmp2->enqueue(tempPair.first);
                }
                if(!IndexGenerator::getInstance()->creatAllIndex(tmp2)) {
                    sucess = false;
                    break;
                }
                tmp2->clear();
            }
            delete tmp2;
            qDebug() << "content index end;";
            if(sucess) {
                IndexStatusRecorder::getInstance()->setStatus(CONTENT_INDEX_DATABASE_STATE, "2");
            }
            sem.release(2);
        });
        //        OCR功能目前合到内容搜索分类中
//        QtConcurrent::run(&m_pool,[&]() {
//            sem.acquire(5);
//            mutex3.unlock();
//            QQueue<QString>* tmpOcr = new QQueue<QString>();
//            qDebug() << "m_ocr_index:" << m_ocr_index->size();
//            if(m_allDatadaseStatus && m_contentIndexDatabaseStatus) {
//                sem.release(2);
//                return;
//            }
//            IndexGenerator::getInstance()->rebuildOcrIndexDatabase();
//            bool sucess = true;
//            while(!this->m_ocr_index->empty()) {
//                qint64 fileSize = 0;
//                //一次处理的数据量文件总大小为50M以下，50M为暂定值
//                for(size_t i = 0;/* (i < 30) && (fileSize < 52428800) && */(!this->m_ocr_index->empty()); ++i) {
//                    QPair<QString,qint64> tempPair = this->m_ocr_index->dequeue();
//                    fileSize += tempPair.second;
//                    if (fileSize > 52428800) {
//                        if (tmpOcr->size() == 0) {
//                            tmpOcr->enqueue(tempPair.first);
//                            break;
//                        }
//                        this->m_ocr_index->enqueue(tempPair);
//                        break;
//                    }
//                    tmpOcr->enqueue(tempPair.first);
//                }
//                if(!IndexGenerator::getInstance()->creatAllIndex(tmpOcr)) {
//                    sucess = false;
//                    break;
//                }
//                tmpOcr->clear();
//            }
//            delete tmpOcr;
//            qDebug() << "OCR index end;";
//            if(sucess) {
//                IndexStatusRecorder::getInstance()->setStatus(OCR_DATABASE_STATE, "2");
//            }
//            sem.release(5);
//        });
        mutex1.lock();
        mutex2.lock();
        //        mutex3.lock();
        sem.acquire(5);
        mutex1.unlock();
        mutex2.unlock();
        //        mutex3.unlock();

        if(this->m_indexData)
            delete this->m_indexData;
        this->m_indexData = nullptr;
        if(this->m_contentIndexData)
            delete this->m_contentIndexData;
        this->m_contentIndexData = nullptr;
        if(this->m_ocrIndexData)
            delete this->m_ocrIndexData;
        this->m_ocrIndexData = nullptr;
        ::_exit(0);
    } else if(pid < 0) {
        qWarning() << "First Index fork error!!";
    } else {
        waitpid(pid, NULL, 0);
        --FileUtils::indexStatus;
    }

    m_semaphore.release(1);
    IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "2");
    //    int retval1 = write(fifo_fd, buffer, strlen(buffer));
    //    if(retval1 == -1) {
    //        qWarning("write error\n");
    //    }
    //    qDebug("write data ok!\n");
    QTime t2 = QTime::currentTime();
    qWarning() << t1;
    qWarning() << t2;

    return;

}
