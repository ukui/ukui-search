#ifndef SEARCHRESULTPAGEVIEW_H
#define SEARCHRESULTPAGEVIEW_H

#include <QObject>
#include <QQuickView>

class SearchResultPageView : public QQuickView
{
    Q_OBJECT
public:
    SearchResultPageView();

public Q_SLOTS:
    void textChangeSlot(QString text);

};

#endif // SEARCHRESULTPAGEVIEW_H
