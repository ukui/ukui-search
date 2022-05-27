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
#include <QMutexLocker>
#include <gio/gdesktopappinfo.h>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDomDocument>
#include "gobject-template.h"
#include "hanzi-to-pinyin.h"

using namespace UkuiSearch;
size_t FileUtils::maxIndexCount = 0;
unsigned short FileUtils::indexStatus = 0;
FileUtils::SearchMethod FileUtils::searchMethod = FileUtils::SearchMethod::DIRECTSEARCH;
QMap<QString, QStringList> FileUtils::map_chinese2pinyin = QMap<QString, QStringList>();
static QMutex iconMutex;

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
    QMutexLocker locker(&iconMutex);
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_ICON,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if(!G_IS_FILE_INFO(info.get()->get()))
        return QIcon::fromTheme("unknown",QIcon(":/res/icons/unknown.svg"));
    GIcon *g_icon = g_file_info_get_icon(info.get()->get());

    //do not unref the GIcon from info.
    if(G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON(g_icon));
        if(icon_names) {
            auto p = icon_names;
            while(*p) {
                QIcon icon = QIcon::fromTheme(*p);
                if(!icon.isNull()) {
                    return icon;
                } else {
                    p++;
                }
            }
        }
    }
    return QIcon::fromTheme("unknown",QIcon(":/res/icons/unknown.svg"));
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
        return QIcon::fromTheme("unknown",QIcon(":/res/icons/unknown.svg"));
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
QIcon FileUtils::getSettingIcon(const QString &setting, const bool is_white) {
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
        return QIcon::fromTheme("ukui-control-center", QIcon(":/res/icons/ukui-control-center.svg")); //无插件图标时，返回控制面板应用图标
//        if (is_white) {
//            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1White.svg").arg("About"));
//        } else {
//            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1.svg").arg("About"));
//        }
    }
}

QIcon FileUtils::getSettingIcon() {
    return QIcon::fromTheme("ukui-control-center", QIcon(":/res/icons/ukui-control-center.svg")); //返回控制面板应用图标
}

/**
 * @brief FileUtils::getFileName 获取文件名
 * @param uri 格式为"file:///home/xxx/xxx/xxxx.txt"
 * @return
 */
QString FileUtils::getFileName(const QString &uri) {
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
QString FileUtils::getAppName(const QString &path) {
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
QString FileUtils::getSettingName(const QString &setting) {
    return setting.right(setting.length() - setting.lastIndexOf("/") - 1);
}

bool FileUtils::isOrUnder(QString pathA, QString pathB)
{
    if(pathA[0] != "/")
        pathA.prepend("/");
    if(pathB[0] != "/")
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
void stitchMultiToneWordsDFS(const QString &hanzi, const QString &resultAllPinYin, const QString &resultFirst, QStringList &resultList) {
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
void stitchMultiToneWordsBFSStack(const QString &hanzi, QStringList &resultList) {
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
void stitchMultiToneWordsBFSHeap(const QString &hanzi, QStringList &resultList) {
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
void stitchMultiToneWordsBFSHeapLess3(const QString &hanzi, QStringList &resultList) {
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
void stitchMultiToneWordsBFSStackLess3(const QString &hanzi, QStringList &resultList) {
    QString tempHanzi;
    QQueue<QString> tempQueue;
    QQueue<QString> tempQueueFirst;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    int multiToneWordNum = 0;

    for (auto i:hanzi) {
        if (HanZiToPinYin::getInstance()->isMultiTone(QString(i).toStdString()))
            ++multiToneWordNum;
    }
    if(multiToneWordNum > 3) {
        QString oneResult, oneResultFirst;
        for(auto i : hanzi) {
            QStringList results;
            HanZiToPinYin::getInstance()->getResults(QString(i).toStdString(), results);
            if(results.size()) {
                oneResult += results.first();
                oneResultFirst += results.first().at(0);
            } else {
                oneResult += i;
                oneResultFirst += i;
            }
        }
        resultList.append(oneResult);
        resultList.append(oneResultFirst);
        return;
    }

    QStringList results;
    HanZiToPinYin::getInstance()->getResults(QString(tempHanzi.at(0)).toStdString(), results);
    if(results.size()) {
        for(auto i : results) {
            tempQueue.enqueue(i);
            tempQueueFirst.enqueue(i.at(0));
        }
    } else {
        tempQueue.enqueue(tempHanzi.at(0));
        tempQueueFirst.enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while(tempHanzi.size() != 0) {
        HanZiToPinYin::getInstance()->getResults(QString(tempHanzi.at(0)).toStdString(), results);
        tempQueueSize = tempQueue.size();
        if(results.size()) {
            for(int j = 0; j < tempQueueSize; ++j) {
                for(auto i : results) {
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
    return;
}

QStringList FileUtils::findMultiToneWords(const QString &hanzi) {
    QStringList output;
    stitchMultiToneWordsBFSStackLess3(hanzi, output);
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

int FileUtils::openFile(QString &path, bool openInDir)
{
    int res = -1;
    if(openInDir) {
        QStringList list;
        list.append(path);
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.FileManager1",
                                                              "/org/freedesktop/FileManager1",
                                                              "org.freedesktop.FileManager1",
                                                              "ShowItems");
        message.setArguments({list, "ukui-search"});
        QDBusMessage messageRes = QDBusConnection::sessionBus().call(message);
        if (QDBusMessage::ReplyMessage == messageRes.ReplyMessage) {
            res = 0;
        } else {
            qDebug() << "Error! QDBusMessage reply error! ReplyMessage:" << messageRes.ReplyMessage;
            res = -1;
        }
    } else {
        auto file = wrapGFile(g_file_new_for_uri(QUrl::fromLocalFile(path).toString().toUtf8().constData()));
        auto fileInfo = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                  "standard::*," "time::*," "access::*," "mountable::*," "metadata::*," "trash::*," G_FILE_ATTRIBUTE_ID_FILE,
                                  G_FILE_QUERY_INFO_NONE,
                                  nullptr,
                                  nullptr));
        QString mimeType = g_file_info_get_content_type (fileInfo.get()->get());
        if (mimeType == nullptr) {
            if (g_file_info_has_attribute(fileInfo.get()->get(), "standard::fast-content-type")) {
                mimeType = g_file_info_get_attribute_string(fileInfo.get()->get(), "standard::fast-content-type");
            }
        }

        GError *error = NULL;
        GAppInfo *info  = NULL;
        /*
        * g_app_info_get_default_for_type function get wrong default app, so we get the
        * default app info from mimeapps.list, and chose the right default app for mimeType file
        */
        QString mimeAppsListPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
           + "/.config/mimeapps.list";
        GKeyFile *keyfile = g_key_file_new();
        gboolean ret = g_key_file_load_from_file(keyfile, mimeAppsListPath.toUtf8(), G_KEY_FILE_NONE, &error);
        if (false == ret) {
            qWarning()<<"load mimeapps list error msg"<<error->message;
            info = g_app_info_get_default_for_type(mimeType.toUtf8().constData(), false);
            g_error_free(error);
        } else {
            gchar *desktopApp = g_key_file_get_string(keyfile, "Default Applications", mimeType.toUtf8(), &error);
            if (NULL != desktopApp) {
                info = (GAppInfo*)g_desktop_app_info_new(desktopApp);
                g_free (desktopApp);
            } else {
                info = g_app_info_get_default_for_type(mimeType.toUtf8().constData(), false);
            }
        }
        g_key_file_free (keyfile);
        if(!G_IS_APP_INFO(info)) {
            res = -1;
        } else {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            res = 0;
        }
        g_object_unref(info);
    }
    return res;
}

bool FileUtils::copyPath(QString &path)
{
    QApplication::clipboard()->setText(path);
    return true;
}

QString FileUtils::escapeHtml(const QString &str)
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}

QString FileUtils::chineseSubString(const std::string &myStr, int start, int length)
{
    std::string afterSub = "";
    //越界保护
    if(start < 0 || length < 0){
        return " ";
    }

    QString sub = QString::fromStdString(myStr);
    QFont ft(QApplication::font().family(),QApplication::font().pointSize());
    QFontMetrics fm (ft);

    if (length >= myStr.length()) {
        afterSub = myStr.substr(start,length);    //截取;
        if (fm.width(QString::fromStdString(afterSub)) >= 2*LABEL_MAX_WIDTH) {
            sub = fm.elidedText(sub, Qt::ElideRight, 2*LABEL_MAX_WIDTH);    //超过两行则省略
        } else {
            sub = fm.elidedText(sub, Qt::ElideLeft, 2*LABEL_MAX_WIDTH);    //超过两行则省略
        }
        return sub;
    }
    if (start + length <= myStr.length()) {
        afterSub = myStr.substr(start,length);    //截取
        sub = QString::fromStdString(afterSub);    //转QString

        if(start + length < myStr.length()){
           sub.replace(sub.length()-3,3,"…");    //替换后三位
        } else{
           sub.append("…");   //直接加
        }
        sub = fm.elidedText(sub, Qt::ElideRight, 2*LABEL_MAX_WIDTH);    //超过两行则省略
    } else {
        int newStart = myStr.length()-length;    //更新截取位置

        afterSub = myStr.substr(newStart, length);
        sub=QString::fromStdString(afterSub);
        if (fm.width(QString::fromStdString(myStr.substr(newStart, start))) >= 2*LABEL_MAX_WIDTH) {
            sub = fm.elidedText(sub, Qt::ElideLeft, 2*LABEL_MAX_WIDTH);
        } else {
            if (newStart + 3 < start) {
                sub.replace(0,3,"…").append("…");
            } else {
                afterSub = myStr.substr(start, length);
                sub = "…" + QString::fromStdString(afterSub);
                sub.append("…");
            }
            sub = fm.elidedText(sub, Qt::ElideRight, 2*LABEL_MAX_WIDTH);
        }
    }
    return sub;
}

QIcon FileUtils::iconFromTheme(const QString &name, const QIcon &iconDefault)
{
    QMutexLocker locker(&iconMutex);
    return QIcon::fromTheme(name, iconDefault);
}

bool FileUtils::isOpenXMLFileEncrypted(QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray encrypt = file.read(4);
    file.close();
    if (encrypt.length() < 4) {
        qDebug() << "Reading file error!" << path;
        return true;
    }
    //比较前四位是否为对应值来判断OpenXML类型文件是否加密
    if (encrypt[0] == 0x50 && encrypt[1] == 0x4b && encrypt[2] == 0x03 && encrypt[3] == 0x04) {
        return false;
    } else {
        qDebug() << "Encrypt!" << path;
        return true;
    }
}
//todo: only support docx, pptx, xlsx
bool FileUtils::isEncrypedOrUnreadable(QString path)
{
    QMimeType type = FileUtils::getMimetype(path);
    QString name = type.name();
    QFileInfo file(path);
    QString strsfx =  file.suffix();
    if(name == "application/zip") {
        if (strsfx == "docx" || strsfx == "pptx" || strsfx == "xlsx") {

            return FileUtils::isOpenXMLFileEncrypted(path);
        } else if (strsfx == "uot" || strsfx == "uos" || strsfx == "uop") {
            return false;

        } else if (strsfx == "ofd") {
            return false;

        } else {
            return true;
        }
    } else if(name == "text/plain") {
        if(strsfx.endsWith("txt"))
            return false;
        return true;
    } else if(name == "text/html") {
        if(strsfx.endsWith("html"))
            return false;
        return true;
    } else if(type.inherits("application/msword") || type.name() == "application/x-ole-storage") {
        if(strsfx == "doc" || strsfx == "dot" || strsfx == "wps" || strsfx == "ppt" ||
                strsfx == "pps" || strsfx == "dps" || strsfx == "et" || strsfx == "xls" || strsfx == "uof") {
            return false;
        }
        return true;
    } else if(name == "application/pdf") {
        if(strsfx == "pdf")
            return false;
        return true;

    } else if(name == "application/xml" || name == "application/uof") {
        if(strsfx == "uof") {
            return false;
        }
        return true;

    } else {
        qWarning() << "Unsupport format:[" << path << "][" << type.name() << "]";
        return true;
    }
}

bool FileUtils::isOcrSupportSize(QString path)
{
/*
    bool res;
    Pix *image = pixRead(path.toStdString().data());
    if (image->h < OCR_MIN_SIZE or image->w < OCR_MIN_SIZE) {//限制图片像素尺寸
        qDebug() << "file:" << path << "is not right size.";
        res = false;
    } else
        res = true;

    pixDestroy(&image);
    return res;
*/
    QImage file(path);
    if (file.height() < OCR_MIN_SIZE or file.width() < OCR_MIN_SIZE) {//限制图片像素尺寸
        qDebug() << "file:" << path << "is not right size.";
        return false;
    } else
        return true;
}

QString FileUtils::getHtmlText(const QString &text, const QString &keyword)
{
    QString htmlString;
    bool boldOpenned = false;
    for(int i = 0; i < text.length(); i++) {
        if((keyword.toUpper()).contains(QString(text.at(i)).toUpper())) {
            if(! boldOpenned) {
                boldOpenned = true;
                htmlString.append(QString("<b><font size=\"4\">"));
            }
            htmlString.append(FileUtils::escapeHtml(QString(text.at(i))));
        } else {
            if(boldOpenned) {
                boldOpenned = false;
                htmlString.append(QString("</font></b>"));
            }
            htmlString.append(FileUtils::escapeHtml(QString(text.at(i))));
        }
    }
    htmlString.replace("\n", "<br />");//替换换行符
    return "<pre>" + htmlString + "</pre>";
}

QString FileUtils::setAllTextBold(const QString &name)
{
    return QString("<h3 style=\"font-weight:normal;\"><pre>%1</pre></h3>").arg(escapeHtml(name));
}

QString FileUtils::wrapData(QLabel *p_label, const QString &text)
{
    QString wrapText = text;

    QFontMetrics fontMetrics = p_label->fontMetrics();
    int textSize = fontMetrics.width(wrapText);

    if(textSize > LABEL_MAX_WIDTH){
        int lastIndex = 0;
        int count = 0;

        for(int i = lastIndex; i < wrapText.length(); i++) {

            if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) == LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i, '\n');
                count++;
            } else if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) > LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i - 1, '\n');
                count++;
            } else {
                continue;
            }

            if(count == 2){
                break;
            }
        }
    }
//    p_label->setText(wrapText);
    return wrapText;
}

/**
 * uof1.0解析
 * 参考规范：GB/T 20916-2007
 * 1.文字处理
 * 2.电子表格
 * 3.演示文稿
 * ppt的内容存放在对象集中，
 * 可以通过演示文稿-主体-幻灯片集-幻灯片下的锚点属性获取引用了哪些内容：
 * <uof:锚点 uof:图形引用="OBJ16"/>
 * 目标：文本串
 */
void FileUtils::getUOFTextContent(QString &path, QString &textContent)
{
    QFileInfo info(path);
    if (!info.exists() || info.isDir()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    bool isPDF = false;
    QDomElement rootElem = doc.documentElement();
    QDomNode node = rootElem.firstChild();
    while (!node.isNull()) {
        QDomElement e = node.toElement();
        if (!e.isNull() && e.tagName() == "uof:演示文稿") {
            isPDF = true;
            break;
        }
        node = node.nextSibling();
    }

    //单独处理pdf文档
    if (isPDF) {
        qDebug() << path << "is PDF";
        return;
    }

    file.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        //适用于文字处理与电子表格
        if (reader.readNextStartElement() && reader.name().toString() == "文本串") {
            textContent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
            if (textContent.length() >= MAX_CONTENT_LENGTH / 3) {
                break;
            }
        }
    }

    file.close();
}

/**
 * uof2.0解析
 * @brief 参考规范文档 https://www.doc88.com/p-9089133923912.html 或 GJB/Z 165-2012
 * ppt文档的内容存放在graphics.xml中，需要先解析content中的引用再解析graphics内容
 * @param path
 * @param textContent
 */
void FileUtils::getUOF2TextContent(QString &path, QString &textContent)
{
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || info.isDir())
        return;

    QuaZip file(path);
    if (!file.open(QuaZip::mdUnzip))
        return;

    if (!file.setCurrentFile("content.xml")) {
        return;
    }

    QuaZipFile fileR(&file);
    if (!fileR.open(QIODevice::ReadOnly)) {
        return;
    }

    QXmlStreamReader reader(&fileR);

    while (!reader.atEnd()) {
        if (reader.readNextStartElement() && reader.name().toString() == "文本串_415B") {
            textContent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
            if (textContent.length() >= MAX_CONTENT_LENGTH / 3) {
                break;
            }
        }
    }

    fileR.close();
    file.close();
}

/**
 * OFD文件解析
 * @brief 参考： GB/T 33190-2016
 * @param path
 * @param textContent
 */
void FileUtils::getOFDTextContent(QString &path, QString &textContent)
{
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || info.isDir())
        return;

    QuaZip zipfile(path);
    if (!zipfile.open(QuaZip::mdUnzip))
        return;

    // GB/T 33190-2016规范定义可以存在多个Doc_x目录，暂时只取第一个目录的内容
    QString prefix("Doc_0/Pages/");
    QStringList fileList;
    for (const auto &file: zipfile.getFileNameList()) {
        if (file.startsWith(prefix)) {
            fileList << file;
        }
    }

    for (int i = 0; i < fileList.count(); ++i) {
        QString filename = prefix + "Page_" + QString::number(i) + "/Content.xml";
        if (!zipfile.setCurrentFile(filename)) {
            continue;
        }

        QuaZipFile fileR(&zipfile);
        fileR.open(QIODevice::ReadOnly);
        QXmlStreamReader reader(&fileR);

        while (!reader.atEnd()) {
            if (reader.readNextStartElement() && reader.name().toString() == "TextCode") {
                textContent.append(reader.readElementText().replace("\n", "").replace("\r", " "));
                if (textContent.length() >= MAX_CONTENT_LENGTH / 3) {
                    fileR.close();
                    zipfile.close();
                    return;
                }
            }
        }

        fileR.close();
    }

    zipfile.close();
}
