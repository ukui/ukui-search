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
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainter>
#include <QtMath>
#include <QEvent>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QKeyEvent>
#include <QGSettings/QGSettings>
#include <QSystemTrayIcon>
#include <QTimer>

#include "index/index-generator.h"
#include "libsearch.h"
#include "create-index-ask-dialog.h"
#include "stacked-widget.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include "xatom-helper.h"
#endif
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
#include "settings-widget.h"
#endif

namespace Zeeker {
class SearchResult;
class MainWindow : public QMainWindow {
    friend class SearchResult;
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /**
     * @brief Load the main window
     */

    // The position which mainwindow shows follow the ukui-panel.
    void moveToPanel();

    // The position which mainwindow shows in the center of screen where the cursor in.
    void centerToScreen(QWidget* widget);
    void initGsettings();

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *watched, QEvent *event) override;
    void initUi();
    void initConnections();

Q_SIGNALS:
    void searchMethodChanged(FileUtils::SearchMethod);
    void webEngineChanged();

public Q_SLOTS:
    /**
     * @brief Monitor screen resolution
     * @param rect: Screen resolution
     */
    void monitorResolutionChange(QRect rect);
    /**
     * @brief Monitor primary screen changes
     * @param screen: Primary screen
     */
    void primaryScreenChangedSlot(QScreen *screen);

    void bootOptionsFilter(QString opt);                              // 过滤终端命令
    void clearSearchResult(); //清空搜索结果
    void trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason);
    void settingsBtnClickedSlot();
    void searchKeywordSlot(const QString&);
    void resizeHeight(int height);

private:

    QWidget *m_widget = nullptr;                  // central widget
    QFrame * m_titleFrame = nullptr;             // Title bar frame
    QHBoxLayout * m_titleLyt = nullptr;          // Title layout
    QLabel * m_iconLabel = nullptr;              // Icon lable
    QLabel * m_titleLabel = nullptr;             // Title lable
    QPushButton * m_settingsBtn = nullptr;           // Menu button
//    StackedWidget * m_stackedWidget = nullptr;   // Stacked widget
//    SearchBarHLayout * m_searchLayout = nullptr; // Search bar layout
//    SeachBarWidget * m_searchWidget = nullptr;   // Search bar
    SeachBarWidget *m_searchBarWidget;
    SearchResultPage *m_searchResultPage;
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    SettingsWidget * m_settingsWidget = nullptr; // Settings Widget
#endif

    QStringList m_dirList;

    QQueue<QString> *m_search_result_file = nullptr;
    QQueue<QString> *m_search_result_dir = nullptr;
    QQueue<QPair<QString, QStringList>> *m_search_result_content = nullptr;

    QSystemTrayIcon *m_sys_tray_icon = nullptr;
    CreateIndexAskDialog * m_askDialog = nullptr;
    bool m_isAskDialogVisible = false;

    QTimer * m_askTimer = nullptr; //询问是否创建索引弹窗弹出的计时器
    QTimer * m_researchTimer = nullptr; //创建索引后重新执行一次搜索的计时器
    bool m_currentSearchAsked = false; //本次搜索是否已经询问过是否创建索引了
    QGSettings * m_search_gsettings = nullptr;

    SearchMethodManager m_searchMethodManager;

    void setSearchMethod(const bool&);
    double getTransparentData();
    void initTimer();
    bool tryHideMainwindow();
    void setSearchMethodConfig(const bool&, const bool&);
};
}

#endif // MAINWINDOW_H
