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
#ifndef SEARCHRESULTPAGE_H
#define SEARCHRESULTPAGE_H

#include <QSplitter>
#include "search-page-section.h"

namespace Zeeker {
class SearchResultPage : public QWidget
{
    Q_OBJECT
public:
    explicit SearchResultPage(QWidget *parent = nullptr);
    ~SearchResultPage() = default;
    void setSize(const int&, const int&);
    void setInternalPlugins();
    void appendPlugin(const QString &plugin_id);
    void pressEnter();
    void pressUp();
    void pressDown();
    bool getSelectedState();
    void sendResizeWidthSignal(int size);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void initUi();
    void initConnections();
    void setupConnectionsForWidget(ResultWidget *);

    QSplitter * m_splitter = nullptr;
    QHBoxLayout * m_hlayout = nullptr;
    ResultArea * m_resultArea = nullptr;
    DetailArea * m_detailArea = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();
    void currentRowChanged(const QString &, const SearchPluginIface::ResultInfo&);
    void effectiveSearch();
    void resizeHeight(int height);
    void resizeWidth(const int &);
    void setSelectionInfo(QString &);
    void getResult();
};
}

#endif // SEARCHRESULTPAGE_H
