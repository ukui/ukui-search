#include "construct-document.h"
#include "file-utils.h"
#include "chinese-segmentation.h"
#include <QDebug>
#include <QThread>

//extern QList<Document> *_doc_list_path;
//extern QMutex  _mutex_doc_list_path;

ConstructDocumentForPath::ConstructDocumentForPath(QVector<QString> list, IndexGenerator *parent)
{
    this->setAutoDelete(true);
    m_list = std::move(list);
}

ConstructDocumentForPath::~ConstructDocumentForPath()
{
}

void ConstructDocumentForPath::run()
{
//    qDebug()<<"ConstructDocumentForPath";
    if (!_doc_list_path)
        _doc_list_path = new QList<Document>;
//    qDebug()<<_doc_list_path->size();
    QString index_text = m_list.at(0);
    QString sourcePath = m_list.at(1);
    index_text = index_text.replace(""," ");
    index_text = index_text.simplified();

    //不带多音字版
//    QString pinyin_text = FileUtils::find(QString(list.at(0)).replace(".","")).replace("", " ").simplified();

    //多音字版
    //现加入首字母
    QStringList pinyin_text_list = FileUtils::findMultiToneWords(QString(m_list.at(0)).replace(".",""));
    for (QString& i : pinyin_text_list){
        i.replace("", " ");
        i = i.simplified();
    }

    QString uniqueterm = QString::fromStdString(FileUtils::makeDocUterm(sourcePath));
    QString upTerm = QString::fromStdString(FileUtils::makeDocUterm(sourcePath.section("/",0,-2,QString::SectionIncludeLeadingSep)));

    Document doc;
    doc.setData(sourcePath);
    doc.setUniqueTerm(uniqueterm);
    doc.addTerm(upTerm);
    doc.addValue(m_list.at(2));
    QStringList temp;
    temp.append(index_text);
    temp.append(pinyin_text_list);
    doc.setIndexText(temp);
//    QMetaObject::invokeMethod(m_indexGenerator,"appendDocListPath",Q_ARG(Document,doc));
    _mutex_doc_list_path.lock();
    _doc_list_path->append(doc);
    _mutex_doc_list_path.unlock();
//    qDebug()<<"ConstructDocumentForPath finish";
    return;
}

ConstructDocumentForContent::ConstructDocumentForContent(QString path,QObject *parent)
{
    this->setAutoDelete(true);
    m_path = std::move(path);
}

ConstructDocumentForContent::~ConstructDocumentForContent()
{

}

void ConstructDocumentForContent::run()
{
//    qDebug() << "ConstructDocumentForContent  currentThreadId()" << QThread::currentThreadId();
    //      构造文本索引的document
    if (!_doc_list_content)
        _doc_list_content = new QList<Document>;
    QString content;
    FileReader::getTextContent(m_path,content);
    QString uniqueterm = QString::fromStdString(FileUtils::makeDocUterm(m_path));
    QString upTerm = QString::fromStdString(FileUtils::makeDocUterm(m_path.section("/",0,-2,QString::SectionIncludeLeadingSep)));

    QVector<SKeyWord> term = ChineseSegmentation::getInstance()->callSegement(content);

    Document doc;
    doc.setData(content);
    doc.setUniqueTerm(uniqueterm);
    doc.addTerm(upTerm);
    doc.addValue(m_path);
    for(int i = 0;i<term.size();++i)
    {
        doc.addPosting(term.at(i).word,term.at(i).offsets,static_cast<int>(term.at(i).weight));

    }

    _mutex_doc_list_content.lock();
    _doc_list_content->append(doc);
    _mutex_doc_list_content.unlock();
    content.clear();
    term.clear();
    return;
}
