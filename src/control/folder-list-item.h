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
#ifndef FOLDERLISTITEM_H
#define FOLDERLISTITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

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
}

#endif // FOLDERLISTITEM_H
