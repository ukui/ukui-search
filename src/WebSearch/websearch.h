#ifndef WEBSEARCH_H
#define WEBSEARCH_H

#include <QPushButton>
#include <QtDBus/QtDBus>

class websearch : public QPushButton
{
    Q_OBJECT
public:
     websearch();

private Q_SLOTS:
     void webSearchTextRefresh(QString);

Q_SIGNALS:

};

#endif
