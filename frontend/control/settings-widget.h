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
#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <libsearch.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include "xatom-helper.h"
#endif

namespace Zeeker {
class FolderListItem : public QWidget {
    Q_OBJECT
public:
    explicit FolderListItem(QWidget *parent = nullptr, const QString &path = 0);
    ~FolderListItem();
    QString getPath();

protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    bool eventFilter(QObject *, QEvent *);

private:
    void initUi();

    QString m_path;

    QVBoxLayout * m_layout = nullptr;
    QWidget * m_widget = nullptr;
    QHBoxLayout * m_widgetlayout = nullptr;
    QLabel * m_iconLabel = nullptr;
    QLabel * m_pathLabel = nullptr;
    QLabel * m_delLabel = nullptr;
Q_SIGNALS:
    void onDelBtnClicked(const QString&);
};

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void setIndexState(bool);
    void setIndexNum(int);
    void showWidget();
    void resetWebEngine();

private:
    void initUi();
    void setupBlackList(const QStringList &);
    void clearLayout(QLayout *);
    void refreshIndexState();
    void paintEvent(QPaintEvent *);
    void resize();
    void showWarningDialog(const int&);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    MotifWmHints m_hints;
#endif

    //标题栏
    QVBoxLayout * m_mainLyt = nullptr;
    QFrame * m_contentFrame = nullptr;
    QVBoxLayout * m_contentLyt = nullptr;
    QFrame * m_titleFrame = nullptr;
    QHBoxLayout * m_titleLyt = nullptr;
    QLabel * m_titleIcon = nullptr;
    QLabel * m_titleLabel = nullptr;
    QPushButton * m_closeBtn = nullptr;

    //设置
    QLabel * m_settingLabel = nullptr;
    //文件索引
    QLabel * m_indexTitleLabel = nullptr;
    QLabel * m_indexStateLabel = nullptr;
    QLabel * m_indexNumLabel = nullptr;
    //文件索引设置（黑名单）
    QLabel * m_indexSettingLabel = nullptr;
    QLabel * m_indexDescLabel = nullptr;
    QFrame * m_indexBtnFrame = nullptr;
    QHBoxLayout * m_indexBtnLyt = nullptr;
    QPushButton * m_addDirBtn = nullptr;
    QScrollArea * m_dirListArea = nullptr;
    QWidget * m_dirListWidget = nullptr;
    QVBoxLayout * m_dirListLyt = nullptr;

    //搜索引擎设置
    QLabel * m_searchEngineLabel = nullptr;
    QLabel * m_engineDescLabel = nullptr;
    QButtonGroup * m_engineBtnGroup = nullptr;
    QFrame * m_radioBtnFrame = nullptr;
    QHBoxLayout * m_radioBtnLyt = nullptr;
    QRadioButton * m_baiduBtn = nullptr;
    QLabel * m_baiduLabel = nullptr;
    QRadioButton * m_sougouBtn = nullptr;
    QLabel * m_sougouLabel = nullptr;
    QRadioButton * m_360Btn = nullptr;
    QLabel * m_360Label = nullptr;

    //取消与确认按钮
    QFrame * m_bottomBtnFrame = nullptr;
    QHBoxLayout * m_bottomBtnLyt = nullptr;
    QPushButton * m_cancelBtn = nullptr;
    QPushButton * m_confirmBtn = nullptr;

    QTimer * m_timer;

    int m_blockdirs = 0; //黑名单文件夹数量

Q_SIGNALS:
    void settingWidgetClosed();
    void webEngineChanged(const QString&);

private Q_SLOTS:
//    void onBtnConfirmClicked();
//    void onBtnCancelClicked();
    void onBtnAddClicked();
    void onBtnDelClicked(const QString&);
    void setWebEngine(const QString&);
};
}

#endif // SETTINGSWIDGET_H
