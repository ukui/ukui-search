#ifndef FILEMODEL_H
#define FILEMODEL_H

#include "file-utils.h"
#include <QObject>
#include <QAbstractListModel>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QIcon>
#include <QMap>
#include <QFont>
#include <QFileInfo>
#include <QDateTime>


class filemodel : public QAbstractListModel
{
    Q_OBJECT
public:

    filemodel();
    int rowCount(const QModelIndex&) const override;
    int columnCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&, int) const override;
    QVariant headerData(int section,Qt::Orientation orientation ,int role)const override;
    void matchstart(const QString &source);
    void run(int row,int column);
    void showResult(QStringList result);

private:
    void matchesChanged();
    QString sourcetext;
    QString peonyText;
    QStringList header;
    FileUtils *fileutils;

    QList<QString> returnresult;
    QList<QString> showresult;

    QList<QString> pathresult;
    QList<QString> openPathresult;

    QList<QString> runresult;
    QList<QPixmap> iconresult;
    QList<QString> m_timeResult;
    QProcess *cmd;
    QMap<QString,QList<QString>> m_showMap;


Q_SIGNALS:
    /**
     * @brief Request update signal
     */
    void requestUpdateSignal(int row);

};

#endif // FILEMODEL_H
