#ifndef WEBSEARCH_H
#define WEBSEARCH_H

#include <QPushButton>
#include <QtDBus/QtDBus>
#include <QString>
#include <QPainter>

class websearch : public QPushButton
{
    Q_OBJECT
public:
     websearch();
     QString searchString;
     QString search;
//     void paintEvent(QPaintEvent *e);

private Q_SLOTS:
     void webSearchTextRefresh(QString);

Q_SIGNALS:

};

#endif
