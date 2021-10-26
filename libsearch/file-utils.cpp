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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#include "file-utils.h"
#include <QXmlStreamReader>
#include <mntent.h>
#include <QStandardPaths>
#include <syslog.h>

using namespace Zeeker;
size_t FileUtils::_max_index_count = 0;
size_t FileUtils::_current_index_count = 0;
unsigned short FileUtils::_index_status = 0;
//FileUtils::SearchMethod FileUtils::searchMethod = FileUtils::SearchMethod::DIRECTSEARCH;
FileUtils::SearchMethod FileUtils::searchMethod = FileUtils::SearchMethod::INDEXSEARCH;
QMap<QString, QStringList> FileUtils::map_chinese2pinyin = QMap<QString, QStringList>();

FileUtils::FileUtils() {
}

std::string FileUtils::makeDocUterm(QString path) {
    return QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Md5).toHex().toStdString();
}

/**
 * @brief FileUtils::getFileIcon 获取文件图标
 * @param uri "file:///home/xxx/xxx/xxxx.txt"格式
 * @param checkValid
 * @return
 */
QIcon FileUtils::getFileIcon(const QString &uri, bool checkValid) {
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_ICON,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if(!G_IS_FILE_INFO(info.get()->get()))
        return QIcon::fromTheme("unknown",QIcon(":res/icons/unknown.png"));
    GIcon *g_icon = g_file_info_get_icon(info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if(G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON(g_icon));
        if(icon_names) {
            auto p = icon_names;
            if(*p)
                icon_name = QString(*p);
            if(checkValid) {
                while(*p) {
                    QIcon icon = QIcon::fromTheme(*p);
                    if(!icon.isNull()) {
                        icon_name = QString(*p);
                        break;
                    } else {
                        p++;
                    }
                }
            }
        }
    }
    if(QIcon::fromTheme(icon_name).isNull()) {
        return QIcon::fromTheme("unknown",QIcon(":res/icons/unknown.png"));
    }
    return QIcon::fromTheme(icon_name);
}

/**
 * @brief FileUtils::getAppIcon 获取应用图标
 * @param path .desktop文件的完整路径
 * @return
 */
QIcon FileUtils::getAppIcon(const QString &path) {
    QByteArray ba;
    ba = path.toUtf8();
    GKeyFile * keyfile;
    keyfile = g_key_file_new();
    if(!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)) {
        g_key_file_free(keyfile);
        return QIcon::fromTheme("unknown",QIcon(":res/icons/unknown.png"));
    }
    QString icon = QString(g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, NULL, NULL));
    g_key_file_free(keyfile);
    if(QIcon::fromTheme(icon).isNull()) {
        return QIcon(":/res/icons/desktop.png");
    }
    return QIcon::fromTheme(icon);
}

/**
 * @brief FileUtils::getSettingIcon 获取设置图标
 * @param setting 设置项传入参数，格式为 About/About->Properties
 * @param is_white 选择是否返回白色图标
 * @return
 */
QIcon FileUtils::getSettingIcon(const QString& setting, const bool& is_white) {
    QString name = setting.left(setting.indexOf("/"));
    if(! name.isEmpty()) {
        name.replace(QString(name.at(0)), QString(name.at(0).toUpper()));
    }
    QString path;
    if(is_white) {
        path = QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1White.svg").arg(name);
    } else {
        path = QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1.svg").arg(name);
    }
    QFile file(path);
    if(file.exists()) {
        return QIcon(path);
    } else {
        return QIcon::fromTheme("ukui-control-center"); //无插件图标时，返回控制面板应用图标
//        if (is_white) {
//            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1White.svg").arg("About"));
//        } else {
//            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1.svg").arg("About"));
//        }
    }
}

/**
 * @brief FileUtils::getFileName 获取文件名
 * @param uri 格式为"file:///home/xxx/xxx/xxxx.txt"
 * @return
 */
QString FileUtils::getFileName(const QString& uri) {
    QFileInfo info(uri);
    if(info.exists()) {
        return info.fileName();
    } else {
        return "Unknown File";
    }
//    QUrl url = uri;
//    if (url.fileName().isEmpty()) {
//        return "Unknown File";
//    }
//    return url.fileName();
}

/**
 * @brief FileUtils::getAppName 获取应用名
 * @param path .destop文件的完整路径
 * @return
 */
QString FileUtils::getAppName(const QString& path) {
    QByteArray ba;
    ba = path.toUtf8();
    GKeyFile * keyfile;
    keyfile = g_key_file_new();
    if(!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)) {
        g_key_file_free(keyfile);
        return "Unknown App";
    }
    QString name = QString(g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, NULL, NULL));
    g_key_file_free(keyfile);
    return name;
}

/**
 * @brief FileUtils::getSettingName 获取设置项名
 * @param setting 设置项传入参数，格式为 About/About->Properties
 * @return
 */
QString FileUtils::getSettingName(const QString& setting) {
    return setting.right(setting.length() - setting.lastIndexOf("/") - 1);
}

bool FileUtils::isOrUnder(QString pathA, QString pathB)
{
    if(!pathA.startsWith("/"))
        pathA.prepend("/");
    if(!pathB.startsWith("/"))
        pathB.prepend("/");

    if(pathA.length() < pathB.length())
        return false;

    if(pathA == pathB || pathA.startsWith(pathB + "/"))
        return true;

    return false;
}


void FileUtils::loadHanziTable(const QString &fileName) {
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug("File: '%s' open failed!", file.fileName().toStdString().c_str());
        return;
    }

    /* 读取汉字对照表文件并转换为QMap存储 */
    while(!file.atEnd()) {
        QString content = QString::fromUtf8(file.readLine());
        FileUtils::map_chinese2pinyin[content.split(" ").last().trimmed()] = content.split(" ").first().split(",");
    }

    file.close();

    return;
}

QMimeType FileUtils::getMimetype(QString &path) {
    QMimeDatabase mdb;
    QMimeType type = mdb.mimeTypeForFile(path, QMimeDatabase::MatchContent);

    return type;
}

//aborted
QString FileUtils::find(const QString &hanzi) {
    //        static QMap<QString, QStringList> map = loadHanziTable("://index/pinyinWithoutTone.txt");
    //        static QMap<QString, QStringList> map;
    QString output;
    QStringList stringList = hanzi.split("");

    /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
    for(const QString &str : stringList) {
        if(FileUtils::map_chinese2pinyin.contains(str))
            output += FileUtils::map_chinese2pinyin[str].first();
        else
            output += str;
    }

    return output;
}

//DFS多音字太多直接GG
void stitchMultiToneWordsDFS(const QString& hanzi, const QString& resultAllPinYin, const QString& resultFirst, QStringList& resultList) {
    if(hanzi.size() == 0) {
        resultList.append(resultAllPinYin);
        resultList.append(resultFirst);
        return;
    }
    if(FileUtils::map_chinese2pinyin.contains(hanzi.at(0))) {
        for(auto i : FileUtils::map_chinese2pinyin[hanzi.at(0)]) {
            stitchMultiToneWordsDFS(hanzi.right(hanzi.size() - 1), resultAllPinYin + i, resultFirst + i.at(0), resultList);
        }
    } else {
        stitchMultiToneWordsDFS(hanzi.right(hanzi.size() - 1), resultAllPinYin + hanzi.at(0), resultFirst + hanzi.at(0), resultList);
    }
}

//BFS+Stack多音字太多会爆栈
void stitchMultiToneWordsBFSStack(const QString& hanzi, QStringList& resultList) {
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString> tempQueue;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
        for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
            tempQueue.enqueue(i);
        }
    } else {
        tempQueue.enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while(tempHanzi.size() != 0) {
        tempQueueSize = tempQueue.size();
        if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
            for(int j = 0; j < tempQueueSize; ++j) {
                for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
                    tempQueue.enqueue(tempQueue.head() + i);
                }
                tempQueue.dequeue();
            }
        } else {
            for(int j = 0; j < tempQueueSize; ++j) {
                tempQueue.enqueue(tempQueue.head() + tempHanzi.at(0));
                tempQueue.dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue.empty()) {
        resultList.append(tempQueue.dequeue());
    }
}
//BFS+Heap，多音字太多会耗尽内存
void stitchMultiToneWordsBFSHeap(const QString& hanzi, QStringList& resultList) {
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString>* tempQueue = new QQueue<QString>;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
        for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
            tempQueue->enqueue(i);
        }
    } else {
        tempQueue->enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while(tempHanzi.size() != 0) {
        tempQueueSize = tempQueue->size();
        if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
            for(int j = 0; j < tempQueueSize; ++j) {
                for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
                    tempQueue->enqueue(tempQueue->head() + i);
                }
                tempQueue->dequeue();
            }
        } else {
            for(int j = 0; j < tempQueueSize; ++j) {
                tempQueue->enqueue(tempQueue->head() + tempHanzi.at(0));
                tempQueue->dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue->empty()) {
        resultList.append(tempQueue->dequeue());
    }
    delete tempQueue;
    tempQueue = nullptr;
}

//BFS+Heap+超过3个多音字只建一个索引，比较折中的方案
void stitchMultiToneWordsBFSHeapLess3(const QString& hanzi, QStringList& resultList) {
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString>* tempQueue = new QQueue<QString>;
    QQueue<QString>* tempQueueFirst = new QQueue<QString>;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    int multiToneWordNum = 0;
    for(auto i : hanzi) {
        if(FileUtils::map_chinese2pinyin.contains(i)) {
            if(FileUtils::map_chinese2pinyin[i].size() > 1) {
                ++multiToneWordNum;
            }
        }
    }
    if(multiToneWordNum > 3) {
        QString oneResult, oneResultFirst;
        for(auto i : hanzi) {
            if(FileUtils::map_chinese2pinyin.contains(i)) {
                oneResult += FileUtils::map_chinese2pinyin[i].first();
                oneResultFirst += FileUtils::map_chinese2pinyin[i].first().at(0);
            } else {
                oneResult += i;
                oneResultFirst += i;
            }
        }
        resultList.append(oneResult);
        resultList.append(oneResultFirst);
        return;
    }

    if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
        for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
            tempQueue->enqueue(i);
            tempQueueFirst->enqueue(i.at(0));
        }
    } else {
        tempQueue->enqueue(tempHanzi.at(0));
        tempQueueFirst->enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while(tempHanzi.size() != 0) {
        tempQueueSize = tempQueue->size();
        if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
            for(int j = 0; j < tempQueueSize; ++j) {
                for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
                    tempQueue->enqueue(tempQueue->head() + i);
                    tempQueueFirst->enqueue(tempQueueFirst->head() + i.at(0));
                }
                tempQueue->dequeue();
                tempQueueFirst->dequeue();
            }
        } else {
            for(int j = 0; j < tempQueueSize; ++j) {
                tempQueue->enqueue(tempQueue->head() + tempHanzi.at(0));
                tempQueueFirst->enqueue(tempQueueFirst->head() + tempHanzi.at(0));
                tempQueue->dequeue();
                tempQueueFirst->dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue->empty()) {
        resultList.append(tempQueue->dequeue());
        resultList.append(tempQueueFirst->dequeue());
    }
    delete tempQueue;
    delete tempQueueFirst;
    tempQueue = nullptr;
    tempQueueFirst = nullptr;
    return;
}

//BFS+Stack+超过3个多音字只建一个索引，比较折中的方案
void stitchMultiToneWordsBFSStackLess3(const QString& hanzi, QStringList& resultList) {
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString> tempQueue;
    QQueue<QString> tempQueueFirst;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    int multiToneWordNum = 0;
    for(auto i : hanzi) {
        if(FileUtils::map_chinese2pinyin.contains(i)) {
            if(FileUtils::map_chinese2pinyin[i].size() > 1) {
                ++multiToneWordNum;
            }
        }
    }
    if(multiToneWordNum > 3) {
        QString oneResult, oneResultFirst;
        for(auto i : hanzi) {
            if(FileUtils::map_chinese2pinyin.contains(i)) {
                oneResult += FileUtils::map_chinese2pinyin[i].first();
                oneResultFirst += FileUtils::map_chinese2pinyin[i].first().at(0);
            } else {
                oneResult += i;
                oneResultFirst += i;
            }
        }
        resultList.append(oneResult);
        resultList.append(oneResultFirst);
        return;
    }

    if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
        for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
            tempQueue.enqueue(i);
            tempQueueFirst.enqueue(i.at(0));
        }
    } else {
        tempQueue.enqueue(tempHanzi.at(0));
        tempQueueFirst.enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while(tempHanzi.size() != 0) {
        tempQueueSize = tempQueue.size();
        if(FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))) {
            for(int j = 0; j < tempQueueSize; ++j) {
                for(auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]) {
                    tempQueue.enqueue(tempQueue.head() + i);
                    tempQueueFirst.enqueue(tempQueueFirst.head() + i.at(0));
                }
                tempQueue.dequeue();
                tempQueueFirst.dequeue();
            }
        } else {
            for(int j = 0; j < tempQueueSize; ++j) {
                tempQueue.enqueue(tempQueue.head() + tempHanzi.at(0));
                tempQueueFirst.enqueue(tempQueueFirst.head() + tempHanzi.at(0));
                tempQueue.dequeue();
                tempQueueFirst.dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue.empty()) {
        resultList.append(tempQueue.dequeue());
        resultList.append(tempQueueFirst.dequeue());
    }
    //    delete tempQueue;
    //    delete tempQueueFirst;
    //    tempQueue = nullptr;
    //    tempQueueFirst = nullptr;
    return;
}

QStringList FileUtils::findMultiToneWords(const QString& hanzi) {
    //    QStringList* output = new QStringList();
    QStringList output;
    QString tempAllPinYin, tempFirst;
    QStringList stringList = hanzi.split("");

    //    stitchMultiToneWordsDFS(hanzi, tempAllPinYin, tempFirst, output);
    stitchMultiToneWordsBFSStackLess3(hanzi, output);
    //    qDebug() << output;
    return output;
}

/**
 * @brief FileUtils::getDocxTextContent
 * @param path: abs path
 * @return docx to QString
 */
void FileUtils::getDocxTextContent(QString &path, QString &textcontent) {
    //fix me :optimized by xpath??
    QFileInfo info = QFileInfo(path);
    if(!info.exists() || info.isDir())
        return;
    QuaZip file(path);
    if(!file.open(QuaZip::mdUnzip))
        return;

    if(!file.setCurrentFile("word/document.xml", QuaZip::csSensitive)) {
        file.close();
        return;
    }
    QuaZipFile fileR(&file);

    fileR.open(QIODevice::ReadOnly);        //读取方式打开

    QXmlStreamReader reader(&fileR);

    while (!reader.atEnd()){
       if(reader.readNextStartElement() and reader.name().toString() == "t"){
           textcontent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
           if(textcontent.length() >= MAX_CONTENT_LENGTH/3){
               break;
           }
       }
    }

    fileR.close();
    file.close();
    return;

/*    //原加载DOM文档方式；
    QDomDocument doc;
    doc.setContent(fileR.readAll());
    fileR.close();
    QDomElement first = doc.firstChildElement("w:document");
    QDomElement body = first.firstChildElement("w:body");
    while(!body.isNull()) {
        QDomElement wp = body.firstChildElement("w:p");
        while(!wp.isNull()) {
            QDomElement wr = wp.firstChildElement("w:r");
            while(!wr.isNull()) {
                QDomElement wt = wr.firstChildElement("w:t");
                textcontent.append(wt.text().replace("\n", "")).replace("\r", " ");
                if(textcontent.length() >= MAX_CONTENT_LENGTH / 3) {
                    file.close();
                    return;
                }
                wr = wr.nextSiblingElement();
            }
            wp = wp.nextSiblingElement();
        }
        body = body.nextSiblingElement();
    }
    file.close();
    return;
*/
}

void FileUtils::getPptxTextContent(QString &path, QString &textcontent) {
    QFileInfo info = QFileInfo(path);
    if(!info.exists() || info.isDir())
        return;
    QuaZip file(path);
    if(!file.open(QuaZip::mdUnzip))
        return;
    QString prefix("ppt/slides/slide");
    QStringList fileList;
    for(QString i : file.getFileNameList()) {
        if(i.startsWith(prefix))
            fileList << i;
    }
    if(fileList.isEmpty()) {
        file.close();
        return;
    }

    for(int i = 0; i < fileList.size(); ++i){
        QString name = prefix + QString::number(i + 1) + ".xml";
        if(!file.setCurrentFile(name)) {
            continue;
        }
        QuaZipFile fileR(&file);
        fileR.open(QIODevice::ReadOnly);

        QXmlStreamReader reader(&fileR);

        while (!reader.atEnd()){
           if(reader.readNextStartElement() and reader.name().toString() == "t"){
               textcontent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
               if(textcontent.length() >= MAX_CONTENT_LENGTH/3){
                   break;
               }
           }
        }
        fileR.close();
    }
    file.close();
    return;

/*
    QDomElement sptree;
    QDomElement sp;
    QDomElement txbody;
    QDomElement ap;
    QDomElement ar;
    QDomDocument doc;
    QDomElement at;
//    QDomNodeList atList;
    for(int i = 0; i < fileList.size(); ++i) {
        QString name = prefix + QString::number(i + 1) + ".xml";
        if(!file.setCurrentFile(name)) {
            continue;
        }
        QuaZipFile fileR(&file);
        fileR.open(QIODevice::ReadOnly);
        doc.clear();
        doc.setContent(fileR.readAll());
        fileR.close();

        //fix me :optimized by xpath??
        //This method looks better but slower,
        //If xml file is very large with many useless node,this method will take a lot of time.

//        atList = doc.elementsByTagName("a:t");
//        for(int i = 0; i<atList.size(); ++i)
//        {
//            at = atList.at(i).toElement();
//            if(!at.isNull())
//            {
//                textcontent.append(at.text().replace("\r","")).replace("\t"," ");
//                if(textcontent.length() >= MAX_CONTENT_LENGTH/3)
//                {
//                    file.close();
//                    return;
//                }
//            }
//        }
        //This is ugly but seems more efficient when handel a large file.
        sptree = doc.firstChildElement("p:sld").firstChildElement("p:cSld").firstChildElement("p:spTree");
        while(!sptree.isNull()) {
            sp = sptree.firstChildElement("p:sp");
            while(!sp.isNull()) {
                txbody = sp.firstChildElement("p:txBody");
                while(!txbody.isNull()) {
                    ap = txbody.firstChildElement("a:p");
                    while(!ap.isNull()) {
                        ar = ap.firstChildElement("a:r");
                        while(!ar.isNull()) {
                            at = ar.firstChildElement("a:t");
                            textcontent.append(at.text().replace("\r", "")).replace("\t", "");
                            if(textcontent.length() >= MAX_CONTENT_LENGTH / 3) {
                                file.close();
                                return;
                            }
                            ar = ar.nextSiblingElement();
                        }
                        ap = ap.nextSiblingElement();
                    }
                    txbody = txbody.nextSiblingElement();
                }
                sp = sp.nextSiblingElement();
            }
            sptree = sptree.nextSiblingElement();
        }
    }
    file.close();
    return;
*/
}

void FileUtils::getXlsxTextContent(QString &path, QString &textcontent) {
    QFileInfo info = QFileInfo(path);
    if(!info.exists() || info.isDir())
        return;
    QuaZip file(path);
    if(!file.open(QuaZip::mdUnzip))
        return;

    if(!file.setCurrentFile("xl/sharedStrings.xml", QuaZip::csSensitive)) {
        file.close();
        return;
    }
    QuaZipFile fileR(&file);

    fileR.open(QIODevice::ReadOnly);

    QXmlStreamReader reader(&fileR);

    while (!reader.atEnd()){
       if(reader.readNextStartElement() and reader.name().toString() == "t"){
           textcontent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
           if(textcontent.length() >= MAX_CONTENT_LENGTH/3){
               break;
           }
       }
    }

    fileR.close();
    file.close();
    return;

/*
    QDomDocument doc;
    doc.setContent(fileR.readAll());
    fileR.close();
    QDomElement sst = doc.firstChildElement("sst");
    QDomElement si;
    QDomElement r;
    QDomElement t;
    while(!sst.isNull()) {
        si = sst.firstChildElement("si");
        while(!si.isNull()) {
            r = si.firstChildElement("r");
            if(r.isNull()) {
                t = si.firstChildElement("t");
            } else {
                t = r.firstChildElement("t");
            }
            if(t.isNull())
                continue;
            textcontent.append(t.text().replace("\r", "").replace("\n", ""));
            if(textcontent.length() >= MAX_CONTENT_LENGTH / 3) {
                file.close();
                return;
            }
            si = si.nextSiblingElement();
        }
        sst = sst.nextSiblingElement();
    }
    file.close();
    return;
*/
}

void FileUtils::getPdfTextContent(QString &path, QString &textcontent) {
    Poppler::Document *doc = Poppler::Document::load(path);
    if(doc->isLocked()) {
        delete doc;
        return;
    }
    const QRectF qf;
    int pageNum = doc->numPages();
    for(int i = 0; i < pageNum; ++i) {
        textcontent.append(doc->page(i)->text(qf).replace("\n", "").replace("\r", " "));
        if(textcontent.length() >= MAX_CONTENT_LENGTH / 3)
            break;
    }
    delete doc;
    return;
}

void FileUtils::getTxtContent(QString &path, QString &textcontent) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QByteArray encodedString = file.read(MAX_CONTENT_LENGTH);

    uchardet_t chardet = uchardet_new();
    if(uchardet_handle_data(chardet, encodedString.constData(), encodedString.size()) != 0)
        qWarning() << "Txt file encoding format detect fail!" << path;

    uchardet_data_end(chardet);
    const char *codec = uchardet_get_charset(chardet);

    if(QTextCodec::codecForName(codec) == 0)
        qWarning() << "Unsupported Text encoding format" << path << QString::fromLocal8Bit(codec);

    QTextStream stream(encodedString, QIODevice::ReadOnly);
    stream.setCodec(codec);
    uchardet_delete(chardet);

    textcontent = stream.readAll().replace("\n", "").replace("\r", " ");

    file.close();
    encodedString.clear();
    chardet = NULL;
    stream.flush();

    return;
}

bool FileUtils::openXMLFileIsEncrypt(QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray encrypt = file.read(4);
    file.close();
    if (encrypt.length() < 4) {
        qDebug() << "Reading file error!";
        return true;
    }
    //比较前四位是否为对应值来判断OpenXML类型文件是否加密
    if (encrypt[0] == 0x50 && encrypt[1] == 0x4b && encrypt[2] == 0x03 && encrypt[3] == 0x04) {
        return false;
    } else {
        qDebug() << "Encrypt!";
        return true;
    }
}

QString FileUtils::chineseSubString(const std::string &data, int start, int length)
{
    std::string afterSub = "";
    //越界保护
    if(start < 0 || length < 0){
        return " ";
    }
    if (length >= data.length()) {
        return QString::fromStdString(data);
    }

    QString snippet = "";
    QFont ft(QApplication::font().family(),QApplication::font().pointSize());
    QFontMetrics fm (ft);

    if (start + length <= data.length()) {
        afterSub = data.substr(start,length);    //截取
        snippet = QString::fromStdString(afterSub);    //转QString

        if(start + length < data.length()){
           snippet.replace(snippet.length()-3,3,"...");    //替换后三位
        }
        snippet = fm.elidedText(snippet, Qt::ElideRight, 2*366);    //超过两行则省略
    } else {
        int newStart = data.length()-length;    //更新截取位置
        int bodyByteCount = 0;
        int letterCount = 0;

        for(int i = newStart;i > 0;i--){
            if(data[i] & 0x80) {
                char tmp = data[i] << 1;
                if(tmp & 0x80){
                    break;
                } else {
                    bodyByteCount++;
                }

            } else {
                letterCount++;
                if(letterCount == 3) {
                    break;
                }

            }
        }
        int correctionValue = letterCount + (bodyByteCount);
        afterSub = data.substr(newStart - correctionValue, length + correctionValue);
        snippet=QString::fromStdString(afterSub);

        switch(letterCount)
        {
        case 0:
            snippet.replace(0,1,".").prepend("..");
            break;
        case 1:
            snippet.replace(0,2,"..").prepend(".");
            break;
        default:
            snippet.replace(0,3,"...");
        }

        snippet = fm.elidedText(snippet, Qt::ElideLeft, 2*500);
    }
    return snippet;
}

bool FileUtils::isDirRemote(QString path)
{
    struct mntent* ent;
    FILE* f;
    f = setmntent("/proc/mounts", "r");
    if (f == NULL) {
        qWarning() << "setmntent failed!!!.";
        syslog(LOG_ERR, "setmntent failed!!\n");
        return false;
    }
    while(NULL != (ent = getmntent(f))) {
        if(QString(ent->mnt_dir) == path && QString(ent->mnt_type).contains("cifs")) {
            qDebug() << path << "mnt_type:" << QString(ent->mnt_type);
            endmntent(f);
            return true;
        }
    }
    endmntent(f);
    return false;
}
