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
#include "../Interface/morebutton.h"
#include "../Interface/headlabel.h"
#include <QPainter>


class SettingWidget : public QWidget
{
    Q_OBJECT
public:
     SettingWidget();
     settingview *settingView;
     settingModel *m_settingmodel;//设置model
     QVBoxLayout *m_listLayout;

private:
     void initSettingsearchUI();
     void paintEvent(QPaintEvent *e);
     bool eventFilter(QObject *watched, QEvent *event);

private:
     MoreButton *m_Button;
     HeadLabel *m_Headlabel;

private Q_SLOTS:
     void settingTextRefresh(QString);
     void recvSettingSearchResult(int row);

public Q_SLOTS:
     void fristSelect();

Q_SIGNALS:
//     void sendSearchKeyword(QString arg);


};

#endif // SETTINGWIDGET_H
