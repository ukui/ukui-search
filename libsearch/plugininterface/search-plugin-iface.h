#ifndef SEARCHPLUGINIFACE_H
#define SEARCHPLUGINIFACE_H
#define SearchPluginIface_iid "org.ukui.ukui-search.plugin-iface.SearchPluginInterface"

#include <QString>
#include <QIcon>
#include <QMap>
#include "plugin-iface.h"

namespace Zeeker {
class SearchPluginIface : public PluginInterface
{
public:
    ///
    /// \brief The resultInfo struct
    ///
    struct resultInfo
    {
        QIcon icon;
        QString name;
        QString description;
        QMap<QString,QString> actionMap;//action name and action key
    };
    virtual ~SearchInterface(){}
    virtual QString getPluginName() = 0;
    virtual void KeywordSearch(QString keyword,QQueue<resultInfo>);
    virtual void openAction(QString key);

};
}
Q_DECLARE_INTERFACE(SearchPluginIface, SearchPluginIface_iid)

#endif // SEARCHPLUGINIFACE_H
