#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <xapian.h>
#include <QString>
#include <QStringList>

class Document
{
public:
    Document();
    ~Document();
    void setData(QString data);
    void addterm(std::string term,int weight =1);
    void addValue(QString value);
    void setUniqueTerm(QString term);
    std::string getUniqueTerm();
    void setIndexText(QStringList indexText);
    QStringList getIndexText();
    Xapian::Document getXapianDocument();
private:
    Xapian::Document *m_document;
    QStringList *m_index_text;
    QString *m_unique_term;

};

#endif // DOCUMENT_H
