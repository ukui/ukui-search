#ifndef SEARCH_H
#define SEARCH_H

#include "libsearch_global.h"

#define signals Q_SIGNALS
#define slots Q_SLOTS

#include "ukui/kappwidgetprovider.h"
#include "ukui/kappwidgetmanager.h"

#undef signals
#undef slots

#include "global-settings.h"
#include <QDBusInterface>

namespace UkuiSearch {
class LIBSEARCH_EXPORT AppWidgetPlugin : public AppWidget::KAppWidgetProvider

{
    Q_OBJECT
public:
    explicit AppWidgetPlugin(QString providername = "search", QObject *parent = nullptr);
    void appWidgetRecevie(const QString &eventname, const QString &widgetname, const QDBusVariant &value);
    void appWidgetUpdate();

Q_SIGNALS:
    void startSearch(QString);

private:
    QDBusInterface* m_interface = nullptr;
    AppWidget::KAppWidgetManager* m_manager = nullptr;
};
}



#endif // SEARCH_H
