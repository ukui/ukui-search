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
 *
 */
#include "construct-document.h"
#include "file-utils.h"
#include "chinese-segmentation.h"
#include <QDebug>
#include <QThread>
#include <QUrl>

//extern QList<Document> *_doc_list_path;
//extern QMutex  _mutex_doc_list_path;
using namespace Zeeker;
ConstructDocumentForPath::ConstructDocumentForPath(QVector<QString> list) {
    this->setAutoDelete(true);
    m_list = std::move(list);
}

void ConstructDocumentForPath::run() {
//    qDebug()<<"ConstructDocumentForPath";
//    if (!Zeeker::_doc_list_path)
//        Zeeker::_doc_list_path = new QVector<Document>;
//    qDebug()<<_doc_list_path->size();
    QString index_text = m_list.at(0).toLower();
    QString sourcePath = m_list.at(1);
    Document doc;

    //多音字版
    //现加入首字母
    QStringList pinyin_text_list = FileUtils::findMultiToneWords(QString(m_list.at(0)).replace(".", ""));
//    if (!pinyin_text_list.isEmpty())
//    {
//        for (QString& i : pinyin_text_list){
//            i.replace("", " ");
//            i = i.simplified();
//        }
//        doc.setIndexText(pinyin_text_list);
//    }

    QString uniqueterm = QString::fromStdString(FileUtils::makeDocUterm(sourcePath));
    QString upTerm = QString::fromStdString("ZEEKERUPTERM" + FileUtils::makeDocUterm(sourcePath.section("/", 0, -2, QString::SectionIncludeLeadingSep)));
//    qDebug()<<"sourcePath"<<sourcePath.section("/",0,-2,QString::SectionIncludeLeadingSep);
//    qDebug() << "sourcePath ---------------------------: " << sourcePath;
//    qDebug() << "sourcePath.section -------------------: " << sourcePath.section("/",0,-2,QString::SectionIncludeLeadingSep);
//    qDebug() << "ConstructDocumentForPath -- uniqueterm: " << uniqueterm;
//    qDebug() << "ConstructDocumentForPath -- upTerm :     " << upTerm;


    doc.setData(sourcePath);
    doc.setUniqueTerm(uniqueterm);
    doc.addTerm(upTerm);
    doc.addValue(m_list.at(2));
    /*    QStringList temp;
    //    temp.append(index_text);
        temp.append(pinyin_text_list)*/;
    int i = 0;
    int postingCount = 1; //terms post of Xapian document is start from 1!
    while(postingCount <= index_text.size()) {
        doc.addPosting(QUrl::toPercentEncoding(index_text.at(i)).toStdString(), postingCount);
        ++postingCount;
    }
    int i = 0;
    for(QString& s : pinyin_text_list) {
        i = 0;
        while(i < s.size()) {
            doc.addPosting(QUrl::toPercentEncoding(s.at(i)).toStdString(), postingCount);
            ++postingCount;
            ++i;
        }
    }

//    QMetaObject::invokeMethod(m_indexGenerator,"appendDocListPath",Q_ARG(Document,doc));
    IndexGenerator::_mutex_doc_list_path.lock();
    IndexGenerator::_doc_list_path.append(doc);
    IndexGenerator::_mutex_doc_list_path.unlock();
//    qDebug()<<"ConstructDocumentForPath finish";
    return;
}

ConstructDocumentForContent::ConstructDocumentForContent(QString path) {
    this->setAutoDelete(true);
    m_path = std::move(path);
}

void ConstructDocumentForContent::run() {
//    qDebug() << "ConstructDocumentForContent  currentThreadId()" << QThread::currentThreadId();
    //      构造文本索引的document
//    if (!Zeeker::_doc_list_content)
//        Zeeker::_doc_list_content = new QVector<Document>;
    QString content;
    FileReader::getTextContent(m_path, content);
    if (content.isEmpty())
        return;
    //QString uniqueterm = QString::fromStdString(FileUtils::makeDocUterm(m_path));
    //QString upTerm = QString::fromStdString(FileUtils::makeDocUterm(m_path.section("/", 0, -2, QString::SectionIncludeLeadingSep)));
    Document doc;
    doc.setData(content);
    //doc.setUniqueTerm(uniqueterm);
    doc.setUniqueTerm(FileUtils::makeDocUterm(m_path));
    //doc.addTerm(upTerm);
    doc.addTerm("ZEEKERUPTERM" + FileUtils::makeDocUterm(m_path.section("/", 0, -2, QString::SectionIncludeLeadingSep)));
    doc.addValue(m_path);

    //'\xEF\xBC\x8C' is "，" "\xE3\x80\x82" is "。"  use three " " to replace ,to ensure the offset info.
    content = content.replace("\t", " ").replace("\xEF\xBC\x8C", "   ").replace("\xE3\x80\x82", "   ");

//    QVector<SKeyWord> term = ChineseSegmentation::getInstance()->callSegement(content.left(20480000));
    std::vector<cppjieba::KeyWord> term = ChineseSegmentation::getInstance()->callSegementStd(content.left(20480000).toStdString());

    for(size_t i = 0; i < term.size(); ++i) {
        doc.addPosting(term.at(i).word, term.at(i).offsets, static_cast<int>(term.at(i).weight));
    }

    IndexGenerator::_mutex_doc_list_content.lock();
    IndexGenerator::_doc_list_content.append(doc);
    IndexGenerator::_mutex_doc_list_content.unlock();
    content.clear();
    content.squeeze();

    term.clear();
    term.shrink_to_fit();
    return;
}
