#include <QTime>
#include <QDebug>
#include "src/mainwindow.h"
#include "inotify-manager.h"
#include "chinesecharacterstopinyin.h"
#include "inotify.h"

void testTraverse(void){
    /*-------------Inotify Test Start---------------*/
//    QTime t1 = QTime::currentTime();
//    InotifyManager* im = new InotifyManager();
//    im->AddWatch("/home");
//    im->Traverse_BFS("/home", true);
//    QTime t2 = QTime::currentTime();
//    qDebug() << t1;
//    qDebug() << t2;
//    im->start();
    /*-------------Inotify Test End-----------------*/

    /*-------------PinyinSearch Test Start---------------*/
//    QTime t1 = QTime::currentTime();
//    QString test("test");
//    qDebug() << IndexGenerator::IndexSearch(test);
//    QTime t2 = QTime::currentTime();
//    qDebug() << t1;
//    qDebug() << t2;
    /*-------------PinyinSearch Test End-----------------*/

    /*-------------InotyifyRefact Test Start---------------*/
//    QTime t1 = QTime::currentTime();
//    InotifyManagerRefact* imr = new InotifyManagerRefact("/home");
//    imr->AddWatch("/home");
//    imr->setPath("/home");
//    imr->Traverse();
//    QTime t2 = QTime::currentTime();
//    qDebug() << t1;
//    qDebug() << t2;
    /*-------------InotyifyRefact Test End-----------------*/



    exit(0);
}

void removeTone(){
    /*-------------Remove Tone Start---------------*/
    qDebug() << chineseCharactersToPinyin::find("z测试策士xl123123");

    QFile file("://index/pinyinWithTone.txt");
    QFile fileOut("/home/zhangzihao/ukui/ns/ukui-search/index/pinyinWithoutTone.txt");
    fileOut.open(QIODevice::WriteOnly/* | QIODevice::Text*/);

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug("File: '%s' open failed!", file.fileName().toStdString().c_str());
        exit(-1);
    }

    while(!file.atEnd()) {
        QString content = QString::fromUtf8(file.readLine());
        content.replace("ā", "a")
                .replace("á", "a")
                .replace("ǎ", "a")
                .replace("à", "a")
                .replace("ō", "o")
                .replace("ó", "o")
                .replace("ǒ", "o")
                .replace("ò", "o")
                .replace("ê", "e")
                .replace("ē", "e")
                .replace("é", "e")
                .replace("ě", "e")
                .replace("è", "e")
                .replace("ī", "i")
                .replace("í", "i")
                .replace("ǐ", "i")
                .replace("ì", "i")
                .replace("ū", "u")
                .replace("ú", "u")
                .replace("ǔ", "u")
                .replace("ù", "u")
                //l和n后面的ü写作v
                .replace("lǖ", "lv")
                .replace("lǘ", "lv")
                .replace("lǚ", "lv")
                .replace("lǜ", "lv")
                .replace("lü", "lv")
                .replace("nǖ", "nv")
                .replace("nǘ", "nv")
                .replace("nǚ", "nv")
                .replace("nǜ", "nv")
                .replace("nü", "nv")
                //l和n后面的ü替换之后，其他的ü替换为u
                .replace("ǖ", "u")
                .replace("ǘ", "u")
                .replace("ǚ", "u")
                .replace("ǜ", "u")
                .replace("ü", "u")
                .replace("ê", "e")
                .replace("ɑ", "a")
                .replace("", "m")
                .replace("ń", "n")
                .replace("", "n")
                .replace("ɡ", "g");
        //去除同音不同调
        //QString content = QString::fromUtf8(file.readLine());

        QStringList temp = content.split(" ").first().split(",").toSet().toList();
        QString outContent;
        for (auto i : temp){
            outContent += i;
            outContent += ",";
        }
        outContent = outContent.left(outContent.size() - 1);
        outContent += " ";
        outContent += content.split(" ").last().trimmed();
        outContent += "\n";
        fileOut.write(outContent.toUtf8());
//        temp.toSet().toList();
        //content.split(" ").first().split(",")

        //map[content.split(" ").last().trimmed()] = content.split(" ").first().split(",");


//        ā á ǎ à ō ó ǒ ò ê ē é ě è ī í ǐ ì ū ú ǔ ù ǖ ǘ ǚ ǜ ü ê ɑ  ń ň  ɡ
//        fileOut.write(content.toUtf8());
        qDebug() << content;
    }

    file.close();
    fileOut.close();
    /*-------------Remove Tone End-----------------*/
}
