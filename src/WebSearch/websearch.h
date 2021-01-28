#ifndef WEBSEARCH_H
#define WEBSEARCH_H

#include <QPushButton>
#include <QtDBus/QtDBus>
#include <QString>
#include <QPainter>
#include <QLabel>
#include <QHBoxLayout>

class websearch : public QPushButton
{
    Q_OBJECT
public:
     websearch();
     QString searchString;
     QString search;
     void paintEvent(QPaintEvent *e);

private:
     QLabel *m_buttonText;
     QHBoxLayout *m_buttonLayout;
     QLabel *m_Iconlabel;

private Q_SLOTS:
     void webSearchTextRefresh(QString);

Q_SIGNALS:

};

#endif
