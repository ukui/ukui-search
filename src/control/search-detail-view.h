/*
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#ifndef SEARCHDETAILVIEW_H
#define SEARCHDETAILVIEW_H

#include <QWidget>
//#include <QWebEngineView>
//#include <QWebEngineSettings>
//#include <QWebEngineProfile>
//#include <QWebEngineUrlRequestInterceptor>
#include <QDesktopServices>
#include "option-view.h"
#include "ukui-search/global-settings.h"

namespace Zeeker {
class SearchDetailView : public QWidget {
    Q_OBJECT
public:
    explicit SearchDetailView(QWidget *parent = nullptr);
    ~SearchDetailView();

    void setupWidget(const int&, const QString&);
    void clearLayout();
    void setContent(const QString&, const QString&);
    bool isEmpty();
    int getType();
    bool isContent = false;
    void setWebWidget(const QString&);
    void setAppWidget(const QString &name, const QString &path, const QString &icon, const QString &description);
    void closeWebWidget();
    bool doubleClickAction(const int&, const QString&);

protected:
    void paintEvent(QPaintEvent *);

private:
    QVBoxLayout * m_layout = nullptr;
    QString m_contentText;
    QString m_keyword;
    bool openAction(const int&, const QString&);
    bool addDesktopShortcut(const QString&);
    bool addPanelShortcut(const QString&);
    bool openPathAction(const QString&);
    bool copyPathAction(const QString&);
    bool installAppAction(const QString&);
    QString getHtmlText(const QString&, const QString&);
    QString escapeHtml(const QString&);
    bool writeConfigFile(const QString&);
    bool m_isEmpty = true;
    int m_type = 0;
    QString m_path = 0;
    QString m_name = 0; //目前只有未安装应用在打开软件商店时需要此参数
    QString m_pkgname = 0; //目前只有未安装应用在打开软件商店时需要此参数

    void initUI();
    QLabel * m_iconLabel = nullptr;
    QFrame * m_nameFrame = nullptr;
    QHBoxLayout * m_nameLayout = nullptr;
    QLabel * m_nameLabel = nullptr;
    QLabel * m_typeLabel = nullptr;
    QFrame * m_hLine = nullptr;
    QFrame * m_detailFrame = nullptr;
    QVBoxLayout * m_detailLyt = nullptr;
    QLabel * m_contentLabel = nullptr;
    QFrame * m_pathFrame = nullptr;
    QFrame * m_timeFrame = nullptr;
    QHBoxLayout * m_pathLyt = nullptr;
    QHBoxLayout * m_timeLyt = nullptr;
    QLabel * m_pathLabel_1 = nullptr;
    QLabel * m_pathLabel_2 = nullptr;
    QLabel * m_timeLabel_1 = nullptr;
    QLabel * m_timeLabel_2 = nullptr;
    QFrame * m_hLine_2 = nullptr;
    OptionView * m_optionView = nullptr;
    QFrame * m_noNetFrame = nullptr;
    QVBoxLayout * m_noNetLyt = nullptr;
    QLabel * m_noNetIconLabel = nullptr;
    QLabel * m_noNetTipsLabel = nullptr;

//    QWebEngineView * m_webView = nullptr;
//    QWebEngineProfile * m_engineProfile = nullptr;
    bool m_reload = false;
    bool m_net_enable = true;
    QString m_currentKeyword;

Q_SIGNALS:
    void configFileChanged();
private Q_SLOTS:
    void execActions(const int&, const int&, const QString&);
};

//此类用于url拦截
//class RequestInterceptor : public QWebEngineUrlRequestInterceptor
//{
//public:
//    explicit RequestInterceptor(QObject * parent = nullptr) : QWebEngineUrlRequestInterceptor(parent) {}
//    virtual void interceptRequest(QWebEngineUrlRequestInfo & info) Q_DECL_OVERRIDE;
//};
}

#endif // SEARCHDETAILVIEW_H
