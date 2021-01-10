#include "file-utils.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QMap>
#include "quazip/quazip.h"
#include <quazip/quazipfile.h>
#include <QDomDocument>
#include <QMimeDatabase>
#include <QMimeType>
#include <QQueue>
size_t FileUtils::_max_index_count = 0;
size_t FileUtils::_current_index_count = 0;
QMap<QString, QStringList> FileUtils::map_chinese2pinyin = QMap<QString, QStringList>();

FileUtils::FileUtils()
{
}

std::string FileUtils::makeDocUterm(QString path)
{
    return QCryptographicHash::hash(path.toUtf8(),QCryptographicHash::Md5).toStdString();
}

/**
 * @brief FileUtils::getFileIcon 获取文件图标
 * @param uri "file:///home/xxx/xxx/xxxx.txt"格式
 * @param checkValid
 * @return
 */
QIcon FileUtils::getFileIcon(const QString &uri, bool checkValid)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                G_FILE_ATTRIBUTE_STANDARD_ICON,
                                                G_FILE_QUERY_INFO_NONE,
                                                nullptr,
                                                nullptr));
    if (!G_IS_FILE_INFO (info.get()->get()))
        return QIcon::fromTheme("unknown");
    GIcon *g_icon = g_file_info_get_icon (info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names) {
            auto p = icon_names;
            if (*p)
                icon_name = QString (*p);
            if (checkValid) {
                while (*p) {
                    QIcon icon = QIcon::fromTheme(*p);
                    if (!icon.isNull()) {
                        icon_name = QString (*p);
                        break;
                    } else {
                        p++;
                    }
                }
            }
        }
    }
    if (QIcon::fromTheme(icon_name).isNull()) {
        return QIcon::fromTheme("unknown");
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
    if (!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)){
        g_key_file_free (keyfile);
        return QIcon::fromTheme("unknown");
    }
    QString icon = QString(g_key_file_get_locale_string(keyfile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, NULL, NULL));
    g_key_file_free(keyfile);
    if (QIcon::fromTheme(icon).isNull()) {
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
    if (! name.isEmpty()) {
        name.replace(QString(name.at(0)), QString(name.at(0).toUpper()));
    }
    QString path;
    if (is_white) {
        path = QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1White.svg").arg(name);
    } else {
        path = QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1.svg").arg(name);
    }
    QFile file(path);
    if (file.exists()) {
        return QIcon(path);
    } else {
        if (is_white) {
            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1White.svg").arg("About"));
        } else {
            return QIcon(QString("/usr/share/ukui-control-center/shell/res/secondaryleftmenu/%1.svg").arg("About"));
        }
    }
}

/**
 * @brief FileUtils::getFileName 获取文件名
 * @param uri 文件的url，格式为"file:///home/xxx/xxx/xxxx.txt"
 * @return
 */
QString FileUtils::getFileName(const QString& uri) {
    QUrl url = uri;
    if (url.fileName().isEmpty()) {
        return "Unknown File";
    }
    return url.fileName();
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
    if (!g_key_file_load_from_file(keyfile, ba.data(), G_KEY_FILE_NONE, NULL)){
        g_key_file_free (keyfile);
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


void FileUtils::loadHanziTable(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
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

QString FileUtils::getMimetype(QString &path, bool getsuffix)
{
    QMimeDatabase mdb;
    QMimeType type = mdb.mimeTypeForFile(path,QMimeDatabase::MatchContent);
    if(getsuffix)
        return type.name();
    else
        return type.preferredSuffix();
}

//aborted
QString FileUtils::find(const QString &hanzi)
{
    //        static QMap<QString, QStringList> map = loadHanziTable("://index/pinyinWithoutTone.txt");
    //        static QMap<QString, QStringList> map;
    QString output;
    QStringList stringList = hanzi.split("");

    /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
    for (const QString &str : stringList) {
        if (FileUtils::map_chinese2pinyin.contains(str))
            output += FileUtils::map_chinese2pinyin[str].first();
        else
            output += str;
    }

    return output;
}

//DFS多音字太多直接GG
void stitchMultiToneWordsDFS(const QString& hanzi, const QString& resultAllPinYin, const QString& resultFirst, QStringList& resultList){
    if (hanzi.size() == 0){
        resultList.append(resultAllPinYin);
        resultList.append(resultFirst);
        return;
    }
    if (FileUtils::map_chinese2pinyin.contains(hanzi.at(0))){
        for (auto i : FileUtils::map_chinese2pinyin[hanzi.at(0)]){
            stitchMultiToneWordsDFS(hanzi.right(hanzi.size() - 1), resultAllPinYin + i, resultFirst + i.at(0), resultList);
        }
    }
    else{
        stitchMultiToneWordsDFS(hanzi.right(hanzi.size() - 1), resultAllPinYin + hanzi.at(0), resultFirst + hanzi.at(0), resultList);
    }
}

//BFS+Stack多音字太多会爆栈
void stitchMultiToneWordsBFSStack(const QString& hanzi, QStringList& resultList){
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString> tempQueue;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
        for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
            tempQueue.enqueue(i);
        }
    }
    else{
        tempQueue.enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while (tempHanzi.size() != 0) {
        tempQueueSize = tempQueue.size();
        if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
            for (int j = 0; j < tempQueueSize; ++j){
                for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
                    tempQueue.enqueue(tempQueue.head() + i);
                }
                tempQueue.dequeue();
            }
        }
        else{
            for (int j = 0; j < tempQueueSize; ++j){
                tempQueue.enqueue(tempQueue.head() + tempHanzi.at(0));
                tempQueue.dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue.empty()){
        resultList.append(tempQueue.dequeue());
    }
}
//BFS+Heap，多音字太多会耗尽内存
void stitchMultiToneWordsBFSHeap(const QString& hanzi, QStringList& resultList){
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString>* tempQueue = new QQueue<QString>;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
        for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
            tempQueue->enqueue(i);
        }
    }
    else{
        tempQueue->enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while (tempHanzi.size() != 0) {
        tempQueueSize = tempQueue->size();
        if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
            for (int j = 0; j < tempQueueSize; ++j){
                for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
                    tempQueue->enqueue(tempQueue->head() + i);
                }
                tempQueue->dequeue();
            }
        }
        else{
            for (int j = 0; j < tempQueueSize; ++j){
                tempQueue->enqueue(tempQueue->head() + tempHanzi.at(0));
                tempQueue->dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue->empty()){
        resultList.append(tempQueue->dequeue());
    }
    delete tempQueue;
    tempQueue = nullptr;
}

//BFS+Heap+超过3个多音字只建一个索引，比较折中的方案
void stitchMultiToneWordsBFSHeapLess3(const QString& hanzi, QStringList& resultList){
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString>* tempQueue = new QQueue<QString>;
    QQueue<QString>* tempQueueFirst = new QQueue<QString>;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    int multiToneWordNum = 0;
    for (auto i : hanzi){
        if (FileUtils::map_chinese2pinyin.contains(i)){
            if (FileUtils::map_chinese2pinyin[i].size() > 1){
                ++multiToneWordNum;
            }
        }
    }
    if (multiToneWordNum > 3){
        QString oneResult, oneResultFirst;
        for (auto i : hanzi){
            if (FileUtils::map_chinese2pinyin.contains(i)){
                oneResult += FileUtils::map_chinese2pinyin[i].first();
                oneResultFirst += FileUtils::map_chinese2pinyin[i].first().at(0);
            }
            else{
                oneResult += i;
                oneResultFirst += i;
            }
        }
        resultList.append(oneResult);
        resultList.append(oneResultFirst);
        return;
    }

    if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
        for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
            tempQueue->enqueue(i);
            tempQueueFirst->enqueue(i.at(0));
        }
    }
    else{
        tempQueue->enqueue(tempHanzi.at(0));
        tempQueueFirst->enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while (tempHanzi.size() != 0) {
        tempQueueSize = tempQueue->size();
        if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
            for (int j = 0; j < tempQueueSize; ++j){
                for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
                    tempQueue->enqueue(tempQueue->head() + i);
                    tempQueueFirst->enqueue(tempQueueFirst->head() + i.at(0));
                }
                tempQueue->dequeue();
                tempQueueFirst->dequeue();
            }
        }
        else{
            for (int j = 0; j < tempQueueSize; ++j){
                tempQueue->enqueue(tempQueue->head() + tempHanzi.at(0));
                tempQueueFirst->enqueue(tempQueueFirst->head() + tempHanzi.at(0));
                tempQueue->dequeue();
                tempQueueFirst->dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue->empty()){
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
void stitchMultiToneWordsBFSStackLess3(const QString& hanzi, QStringList& resultList){
    QString tempHanzi, resultAllPinYin, resultFirst;
    QQueue<QString> tempQueue;
    QQueue<QString> tempQueueFirst;
    tempHanzi = hanzi;
    int tempQueueSize = 0;
    int multiToneWordNum = 0;
    for (auto i : hanzi){
        if (FileUtils::map_chinese2pinyin.contains(i)){
            if (FileUtils::map_chinese2pinyin[i].size() > 1){
                ++multiToneWordNum;
            }
        }
    }
    if (multiToneWordNum > 3){
        QString oneResult, oneResultFirst;
        for (auto i : hanzi){
            if (FileUtils::map_chinese2pinyin.contains(i)){
                oneResult += FileUtils::map_chinese2pinyin[i].first();
                oneResultFirst += FileUtils::map_chinese2pinyin[i].first().at(0);
            }
            else{
                oneResult += i;
                oneResultFirst += i;
            }
        }
        resultList.append(oneResult);
        resultList.append(oneResultFirst);
        return;
    }

    if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
        for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
            tempQueue.enqueue(i);
            tempQueueFirst.enqueue(i.at(0));
        }
    }
    else{
        tempQueue.enqueue(tempHanzi.at(0));
        tempQueueFirst.enqueue(tempHanzi.at(0));
    }
    tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    while (tempHanzi.size() != 0) {
        tempQueueSize = tempQueue.size();
        if (FileUtils::map_chinese2pinyin.contains(tempHanzi.at(0))){
            for (int j = 0; j < tempQueueSize; ++j){
                for (auto i : FileUtils::map_chinese2pinyin[tempHanzi.at(0)]){
                    tempQueue.enqueue(tempQueue.head() + i);
                    tempQueueFirst.enqueue(tempQueueFirst.head() + i.at(0));
                }
                tempQueue.dequeue();
                tempQueueFirst.dequeue();
            }
        }
        else{
            for (int j = 0; j < tempQueueSize; ++j){
                tempQueue.enqueue(tempQueue.head() + tempHanzi.at(0));
                tempQueueFirst.enqueue(tempQueueFirst.head() + tempHanzi.at(0));
                tempQueue.dequeue();
                tempQueueFirst.dequeue();
            }
        }
        tempHanzi = tempHanzi.right(tempHanzi.size() - 1);
    }
    while(!tempQueue.empty()){
        resultList.append(tempQueue.dequeue());
        resultList.append(tempQueueFirst.dequeue());
    }
    //    delete tempQueue;
    //    delete tempQueueFirst;
    //    tempQueue = nullptr;
    //    tempQueueFirst = nullptr;
    return;
}

QStringList FileUtils::findMultiToneWords(const QString& hanzi)
{
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
QString *FileUtils::getDocxTextContent(QString &path)
{
    QFileInfo info = QFileInfo(path);
    if(!info.exists()||info.isDir())
        return nullptr;
    QuaZip file(path);
    if(!file.open(QuaZip::mdUnzip))
        return nullptr;

    if(!file.setCurrentFile("word/document.xml",QuaZip::csSensitive))
        return nullptr;
    QuaZipFile fileR(&file);

    fileR.open(QIODevice::ReadOnly);        //读取方式打开

    QString *allText = new QString();
    QDomDocument doc;
    doc.setContent(fileR.readAll());
    QDomElement first = doc.firstChildElement("w:document");
    first = first.firstChildElement().firstChildElement();
    while(!first.isNull())
    {
        QDomElement wr= first.firstChildElement("w:r");
        while(!wr.isNull())
        {
            QDomElement wt = wr.firstChildElement("w:t");
            allText->append(wt.text());
            wr = wr.nextSiblingElement();
        }
        first = first.nextSiblingElement();
    }
    return allText;
}

QString *FileUtils::getTxtContent(QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return nullptr;
    QString *allText = new QString(file.readAll());
    return allText;
}
