#ifndef CHINESECHARACTERSTOPINYIN_H
#define CHINESECHARACTERSTOPINYIN_H

#include <QObject>
#include <QMap>
#include <QFile>

class chineseCharactersToPinyin : public QObject
{
    Q_OBJECT
public:
    explicit chineseCharactersToPinyin(QObject *parent = nullptr);
    static QString find(const QString &hanzi)
    {
        static QMap<QString, QStringList> map = loadHanziTable("://index/pinyinWithoutTone.txt");
        QString output;
        QStringList stringList = hanzi.split("");

        /* 遍历查找汉字-拼音对照表的内容并将汉字替换为拼音 */
        for (const QString &str : stringList) {
            if (map.contains(str))
                output += map[str].first();
            else
                output += str;
        }

        return output;
    }

Q_SIGNALS:
private:
    /* 加载汉字对照表 */
    static QMap<QString, QStringList> loadHanziTable(const QString &fileName)
    {
        QMap<QString, QStringList> map;
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug("File: '%s' open failed!", file.fileName().toStdString().c_str());
            return map;
        }

        /* 读取汉字对照表文件并转换为QMap存储 */
        while(!file.atEnd()) {
            QString content = QString::fromUtf8(file.readLine());
            map[content.split(" ").last().trimmed()] = content.split(" ").first().split(",");
        }

        file.close();

        return map;
    }
};

#endif // CHINESECHARACTERSTOPINYIN_H
