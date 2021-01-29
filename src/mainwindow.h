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
#include "content-widget.h"
#include "input-box.h"
#include "index/index-generator.h"
#include "settings-widget.h"
#include "libsearch.h"

class SearchResult;
class MainWindow : public QMainWindow
{
    friend class SearchResult;
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /**
     * @brief Load the main window
     */
    void searchContent(QString searchcontent);
    void moveToPanel();

private:
    bool nativeEvent(const QByteArray&, void *, long *);

    QFrame * m_line = nullptr;//Vertical dividing line
    QFrame * m_frame = nullptr;

    QFrame * m_titleFrame = nullptr;//标题栏
    QHBoxLayout * m_titleLyt = nullptr;
    QLabel * m_iconLabel = nullptr;
    QLabel * m_titleLabel = nullptr;
    QPushButton * m_menuBtn = nullptr;
    SettingsWidget * m_settingsWidget = nullptr;

    ContentWidget * m_contentFrame = nullptr;//内容栏

    SeachBarWidget * m_searchWidget = nullptr;//搜索栏
    SearchBarHLayout * m_searchLayout = nullptr;

    bool m_winFlag = false;

    QGSettings * m_transparency_gsettings = nullptr;
    double getTransparentData();

    QVector<QStringList> m_app_setting_lists;
    QStringList m_dirList;

    QQueue<QString> *m_search_result_file = nullptr;
    QQueue<QString> *m_search_result_dir = nullptr;
    QQueue<QPair<QString,QStringList>> *m_search_result_content = nullptr;
    SearchResult * m_search_result_thread = nullptr;

    FileSearcher* m_searcher = nullptr;
    QSystemTrayIcon *m_sys_tray_icon;

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);
    void initUi();

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
};

#endif // MAINWINDOW_H
