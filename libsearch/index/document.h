#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <xapian.h>
#include <QString>
#include <QStringList>
#include <QVector>

class Document
{
public:
    Document();
    ~Document(){}
    Document(const Document& other){
        m_document = other.m_document;
        m_index_text = other.m_index_text;
        m_unique_term = other.m_unique_term;
    }
    void operator=(const Document& other){
            m_document = other.m_document;
            m_index_text = other.m_index_text;
            m_unique_term = other.m_unique_term;
    }
    void setData(QString data);
    void addPosting(std::string term, QVector<size_t> offset, int weight =1);
    void addTerm(QString term);
    void addValue(QString value);
    void setUniqueTerm(QString term);
    std::string getUniqueTerm();
    void setIndexText(QStringList indexText);
    QStringList getIndexText();
    Xapian::Document getXapianDocument();
private:
    Xapian::Document m_document;
    QStringList m_index_text;
    QString m_unique_term;

};

#endif // DOCUMENT_H
