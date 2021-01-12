#include "document.h"
#include <QDebug>

Document::Document()
{
    m_document = new Xapian::Document;
}

Document::~Document()
{
//    if(m_document)
//        delete m_document;
//    if(m_index_text)
//        delete m_index_text;
//    if(m_unique_term)
//        delete m_unique_term;
}

void Document::setData(QString data)
{
    if(data.isEmpty())
        return;
    m_document->set_data(data.toStdString());
}

void Document::addterm(std::string term,QVector<size_t> offset, int weight)
{
    if(term == "")
        return;
    if(term.length() > 240)
        term = QString::fromStdString(term).left(30).toStdString();

    for(size_t i : offset)
    {
        m_document->add_posting(term,i,weight);
    }
}

void Document::addValue(QString value)
{
    m_document->add_value(1,value.toStdString());
}

void Document::setUniqueTerm(QString term)
{
    if(term.isEmpty())
        return;
    m_document->add_term(term.toStdString());

    m_unique_term = new QString(term);
}

std::string Document::getUniqueTerm()
{
//    qDebug()<<"m_unique_term!"<<*m_unique_term;
    return m_unique_term->toStdString();
}

void Document::setIndexText(QStringList indexText)
{
//    QStringList indexTextList = indexText;
    m_index_text = new QStringList(indexText);
}

QStringList Document::getIndexText()
{
    return *m_index_text;
}

Xapian::Document Document::getXapianDocument()
{
    return *m_document;
}
