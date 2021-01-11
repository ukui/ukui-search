#ifndef SEARCHITEM_H
#define SEARCHITEM_H

#include <QObject>
#include <QPixmap>
#include "search-item-model.h"
#include "file-utils.h"

class SearchItem : public QObject
{
    friend class SearchItemModel;
    friend class SearchListView;
    Q_OBJECT
public:
    explicit SearchItem(QObject *parent = nullptr);
    ~SearchItem();

    enum SearchType {
        All,
        Apps,
        Settings,
        Files,
        Dirs,
        Contents,
        Best
    };

    void setSearchList(const int&, const QStringList&);
    void appendItem(QString);
    void removeItem(QString);
    int getCurrentSize();
    void clear();

private:
//    SearchItem * m_parent = nullptr;
//    QVector<SearchItem*> * m_children = nullptr;

    int m_searchtype = 0;
    QStringList m_pathlist;

    QIcon getIcon(int);
    QString getName(int);

Q_SIGNALS:

};

#endif // SEARCHITEM_H
