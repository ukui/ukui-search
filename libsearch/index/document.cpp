#include "document.h"
#include <QDebug>

void Document::setData(QString data)
{
    if(data.isEmpty())
        return;
    m_document.set_data(data.toStdString());
}

void Document::addPosting(std::string term,QVector<size_t> offset, int weight)
{
    if(term == "")
        return;
    if(term.length() > 240)
        term = QString::fromStdString(term).left(30).toStdString();

    for(size_t i : offset)
    {
        m_document.add_posting(term,i,weight);
    }
}

void Document::addTerm(QString term)
{
    if(term.isEmpty())
        return;
    m_document.add_term(term.toStdString());
}

void Document::addValue(QString value)
{
    m_document.add_value(1,value.toStdString());
}

void Document::setUniqueTerm(QString term)
{
    if(term.isEmpty())
        return;
    m_document.add_term(term.toStdString());

//    m_unique_term = new QString(term);
    m_unique_term = std::move(term);
}
std::string Document::getUniqueTerm()
{
//    qDebug()<<"m_unique_term!"<<*m_unique_term;
//    qDebug() << QString::fromStdString(m_unique_term.toStdString());
    return m_unique_term.toStdString();
}

void Document::setIndexText(QStringList indexText)
{
//    QStringList indexTextList = indexText;
//    m_index_text = new QStringList(indexText);
    m_index_text = std::move(indexText);
}

QStringList Document::getIndexText()
{
    return m_index_text;
}

Xapian::Document Document::getXapianDocument()
{
    return m_document;
}
