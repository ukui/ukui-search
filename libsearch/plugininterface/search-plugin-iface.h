#ifndef SEARCHPLUGINIFACE_H
#define SEARCHPLUGINIFACE_H
#define SearchPluginIface_iid "org.ukui.ukui-search.plugin-iface.SearchPluginInterface"

#include <QString>
#include <QIcon>
#include <QList>
#include <QMutex>
#include <QtPlugin>
#include "plugin-iface.h"
#include "data-queue.h"

namespace UkuiSearch {
class SearchPluginIface : public PluginInterface
{
public:
    struct DescriptionInfo
    {
        QString key;
        QString value;
    };
    struct Actioninfo
    {
        int actionkey;
        QString displayName;
    };
    /**
     * @brief The ResultInfo struct
     */
    struct ResultInfo
    {
        QIcon icon;
        QString name;
        QVector<DescriptionInfo> description;
        QString actionKey;
        int type;
    };

    virtual ~SearchPluginIface() {}
    virtual QString getPluginName() = 0;
    virtual void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult) = 0;
    virtual void stopSearch() = 0;
    virtual QList<Actioninfo> getActioninfo(int type) = 0;
    virtual void openAction(int actionkey, QString key, int type) = 0;
//    virtual bool isPreviewEnable(QString key, int type) = 0;
//    virtual QWidget *previewPage(QString key, int type, QWidget *parent = nullptr) = 0;
    virtual QWidget *detailPage(const ResultInfo &ri) = 0;
};
}

Q_DECLARE_INTERFACE(UkuiSearch::SearchPluginIface, SearchPluginIface_iid)

#endif // SEARCHPLUGINIFACE_H
