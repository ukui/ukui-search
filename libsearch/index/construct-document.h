#ifndef CONSTRUCTDOCUMENT_H
#define CONSTRUCTDOCUMENT_H

#include <QRunnable>
#include <QVector>
//#include <QMetaObject>
#include "document.h"
#include "index-generator.h"
//extern QList<Document> *_doc_list_path;
//extern QMutex  _mutex_doc_list_path;
class IndexGenerator;
class ConstructDocumentForPath : public QRunnable
{
public:
    explicit ConstructDocumentForPath(QVector<QString> list,IndexGenerator *parent = nullptr);
    ~ConstructDocumentForPath();
protected:
    void run();
private:
    QVector<QString> m_list;


};

class ConstructDocumentForContent : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ConstructDocumentForContent(QString path,QObject *parent = nullptr);
    ~ConstructDocumentForContent();
protected:
    void run();
private:
    QString m_path;


};

#endif // CONSTRUCTDOCUMENT_H
