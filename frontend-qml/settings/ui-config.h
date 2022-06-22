//
// Created by hxf on 22-6-16.
//

#ifndef UKUI_SEARCH_UI_CONFIG_H
#define UKUI_SEARCH_UI_CONFIG_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QMap>

namespace UkuiSearch {

class UIConfig : public QObject
{
    Q_OBJECT
public:
    static UIConfig *getInstance(QObject *parent = nullptr);

    Q_INVOKABLE QString color(const QString &key);
    Q_INVOKABLE int width(const QString &key);
    Q_INVOKABLE int height(const QString &key);
    Q_INVOKABLE QSize size(const QString &key);
    Q_INVOKABLE int radius(const QString &key);
    Q_INVOKABLE int margin(const QString &key, int pos);
    Q_INVOKABLE int spacing(const QString &key);

private:
    explicit UIConfig(QObject *parent = nullptr);
    void loadConfig();

    struct Margin {
        int left;
        int top;
        int right;
        int bottom;
    };

private:
    QMap<QString, QString> m_colors;
    QMap<QString, QSize> m_size;
    QMap<QString, int> m_radius;
    QMap<QString, int> m_spacing;
    QMap<QString, Margin> m_margins;
};

}

#endif //UKUI_SEARCH_UI_CONFIG_H
