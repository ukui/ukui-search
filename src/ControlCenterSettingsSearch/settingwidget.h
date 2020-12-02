#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QtDBus/QtDBus>
#include <QDebug>
#include <QString>
#include "settingmodel.h"
#include "settingview.h"
#include <QPainter>

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
     SettingWidget();
     QLabel *settingHead;
     settingview *settingView;
     settingModel *m_settingmodel;//设置model
     QVBoxLayout *m_listLayout;

private:
     void initSettingsearchUI();
     void paintEvent(QPaintEvent *e);

private Q_SLOTS:
     void settingTextRefresh(QString);
     void recvSettingSearchResult(int row);

Q_SIGNALS:
//     void sendSearchKeyword(QString arg);


};

#endif // SETTINGWIDGET_H
