#include "search.h"

#include <QDBusPendingCall>
#include <QDBusMetaType>
#include <QTranslator>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>

#define CONTROL_CENTER_PERSONALISE_GSETTINGS_ID "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY "transparency"
#define THEME_GSETTINGS_ID "org.ukui.style"
#define STYLE_NAME_KEY "styleName"

using namespace UkuiSearch;

AppWidgetPlugin::AppWidgetPlugin(QString providername, QObject *parent) : KAppWidgetProvider(providername, parent)
{
    QTranslator* translator = new QTranslator(this);
    if(!translator->load("/usr/share/appwidget/search/translations/" + QLocale::system().name()+ ".qm")) {
        qWarning() << "/usr/share/appwidget/search/translations/" + QLocale::system().name() << "load failed";
    }
    QApplication::installTranslator(translator);

    m_interface = new QDBusInterface("com.ukui.search.service",
                                     "/",
                                     "org.ukui.search.service");

    m_manager = new AppWidget::KAppWidgetManager(this);

    connect(GlobalSettings::getInstance(), &GlobalSettings::transparencyChanged, [ & ] {
        qDebug() << "transparency changed";
        this->appWidgetUpdate();
    });

    connect(GlobalSettings::getInstance(), &GlobalSettings::styleChanged, [ & ] {
        qDebug() << "style changed";
        this->appWidgetUpdate();
    });
}

void AppWidgetPlugin::appWidgetRecevie(const QString &eventname, const QString &widgetname, const QDBusVariant &value)
{
    qDebug() << widgetname << value.variant();
    if (eventname == "enter" or eventname == "return") {
        QString keyword = value.variant().toString();
        Q_EMIT this->startSearch(keyword);
    } else {
        qWarning() << "event:" << eventname << "has no trigger now.";
    }
}

void AppWidgetPlugin::appWidgetUpdate()
{
    QString theme = GlobalSettings::getInstance()->getValue(STYLE_NAME_KEY).toString();
    QJsonObject obj;
    if (theme == "ukui-dark") {
        obj.insert("red", QJsonValue(0));
        obj.insert("green", QJsonValue(0));
        obj.insert("blue", QJsonValue(0));
        obj.insert("placeHolderTextColor", QJsonValue("#72ffffff"));
    } else {
        obj.insert("red", QJsonValue(255));
        obj.insert("green", QJsonValue(255));
        obj.insert("blue", QJsonValue(255));
        obj.insert("placeHolderTextColor", QJsonValue("#72000000"));
    }

    obj.insert("alpha", QJsonValue(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble()));
    QString jsonData = QString(QJsonDocument(obj).toJson());

    QDBusVariant variant;
    variant.setVariant(jsonData);

//    variant.setVariant(QGuiApplication::palette().color(QPalette::Window).name(QColor::HexArgb));
    if(m_manager) {
        m_manager->updateAppWidget("search", variant);
        qDebug() <<"==========update style and transparency successful.";
    }
}
