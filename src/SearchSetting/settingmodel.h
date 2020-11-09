#ifndef SETTINGMODEL_H
#define SETTINGMODEL_H

#include <QObject>
#include <QTimer>
#include <QAbstractListModel>
#include <QSettings>
#include <QDebug>
#include <QStringList>
#include <QList>
#include <QProcess>
#include <QXmlStreamReader>
#include <QHash>
#include <QMap>
#include <QDomElement>
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QIcon>
#include <QPixmap>
class settingModel : public QAbstractListModel
{
    Q_OBJECT
public:
    settingModel();
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex&, int) const override;
    QVariant headerData(int section,Qt::Orientation orientation ,int role)const override;
    void run(int index);
    void matchstart(const QString &source);


private:
    void XmlElement();
    void matching();
    void matchesChanged();

    QString sourcetext;
    QStringList keylist;
    QList<QString> settinglist;
    QTimer *startmatchTimer;
    QTimer *finishtimer;
    QSettings *setting;
    QString settingkey;

    QMap<QString,QStringList> searchlist;
    QStringList searchresult;
    QString index;
    QStringList regmatch;
    QList<QString> returnresult;
    QList<QString> commandresult;
    QList<QString> lockresult;
    QList<QIcon> iconresult;



Q_SIGNALS:
    /**
     * @brief Request update signal
     */
    void requestUpdateSignal(int row);


};

#endif // SETTINGMODEL_H
