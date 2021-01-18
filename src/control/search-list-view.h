#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QObject>
#include <QTreeView>
#include "model/search-item-model.h"
#include "model/search-item.h"
#include "highlight-item-delegate.h"

class SearchListView : public QTreeView
{
    Q_OBJECT
public:
    explicit SearchListView(QWidget *, const QStringList&, const int&);
    ~SearchListView();

    enum ResType { //搜索结果可能出现的类型：应用、文件、设置、文件夹
        Best,
        App,
        Setting,
        Dir,
        File,
        Content
    };

    int getCurrentType();
    static int getResType(const QString&);

    bool is_current_list = false;
    int rowheight = 0;

    void appendItem(QString);
    void setList(QStringList);
    void removeItem(QString);
    void clear();
    void setKeyword(QString);
    int getType();
    int getLength();
    bool isHidden = false;
private:
    SearchItemModel * m_model = nullptr;
    SearchItem * m_item = nullptr;

    HighlightItemDelegate * m_styleDelegate = nullptr;

    int m_type;

Q_SIGNALS:
    void currentRowChanged(const int&, const QString&);

public Q_SLOTS:
    void clearSelection();
};

#endif // SEARCHLISTVIEW_H
