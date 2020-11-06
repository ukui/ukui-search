#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDir>
#include <QDebug>
#include <QRegExp>
#include <QTimer>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>

class filemodel : public QAbstractListModel
{
    Q_OBJECT
public:
    filemodel();
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&, int) const override;
    QVariant headerData(int section,Qt::Orientation orientation ,int role)const override;
    void matchstart(const QString &source);
    void run(int index);

private:
    void matching();
    void matchesChanged();
    void commandsearch();
    void showResult(QString result);
    QTimer *startmatchTimer;
    QString sourcetext;
    QList<QString> returnresult;
    QList<QString> pathresult;
    QList<QString> runresult;
    QList<QString> lockresult;
    QProcess *cmd;
};

#endif // FILEMODEL_H
