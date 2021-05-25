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
#ifndef OPTIONVIEW_H
#define OPTIONVIEW_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "search-list-view.h"

#define NORMAL_COLOR QColor(55, 144, 250, 255)
#define HOVER_COLOR QColor(64, 169, 251, 255)
#define PRESS_COLOR QColor(41, 108, 217, 255)

namespace Zeeker {
class OptionView : public QWidget {
    Q_OBJECT
public:
    explicit OptionView(QWidget *);
    ~OptionView();
    void setupOptions(const int&, bool is_appInstalled = true);

    enum Options {
        Open,
        Shortcut,
        Panel,
        OpenPath,
        CopyPath,
        Install
    };

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    void initUI();
    void setupAppOptions(bool);
    void setupFileOptions();
    void setupDirOptions();
    void setupSettingOptions();
    void setupOptionLabel(const int&);
    void hideOptions();

    int m_type;

    QFrame * m_optionFrame = nullptr;
    QVBoxLayout * m_optionLyt = nullptr;
    QVBoxLayout * m_mainLyt = nullptr;
    QLabel * m_openLabel = nullptr;
    QLabel * m_shortcutLabel = nullptr;
    QLabel * m_panelLabel  = nullptr;
    QLabel * m_openPathLabel = nullptr;
    QLabel * m_copyPathLabel = nullptr;
    QLabel * m_installLabel = nullptr;

Q_SIGNALS:
    void onOptionClicked(const int&);
};
}

#endif // OPTIONVIEW_H
