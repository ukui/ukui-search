#ifndef TEXTCONTENTINDEXER_H
#define TEXTCONTENTINDEXER_H

#include "document.h"
#include <QObject>
#include <QStringList>
#include <QString>

class TextContentIndexer : public QObject
{
    Q_OBJECT
public:
    explicit TextContentIndexer(QObject *parent = nullptr);
    void setFileList(QStringList *filelist);
    void begin();
    bool getPlaintextFileContent(QString path);
Q_SIGNALS:
    bool finish();
private:
    void creatContentdata();
    QStringList *m_file_list;
    Document *m_current_document;


Q_SIGNALS:

};

#endif // TEXTCONTENTINDEXER_H
