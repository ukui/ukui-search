#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>

class FileReader : public QObject
{
    Q_OBJECT
public:
    explicit FileReader(QObject *parent = nullptr);
    static void getTextContent(QString path, QString &textContent);

};

#endif // FILEREADER_H
