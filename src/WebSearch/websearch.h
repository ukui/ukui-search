#ifndef WEBSEARCH_H
#define WEBSEARCH_H

#include <QPushButton>
#include <QtDBus/QtDBus>
#include <QString>
#include <QPainter>
#include <QLabel>
#include <QHBoxLayout>

class webwidget : public QWidget
{
    Q_OBJECT
public:
    webwidget();
protected:
    void paintEvent(QPaintEvent *e);
private Q_SLOTS:
    void webSearchTextRefresh(QString mSearchText);

};

class websearch : public QPushButton
{
    Q_OBJECT
public:
     websearch(QWidget *parent=nullptr);
     QString searchString;
     QString search;

private:
     QLabel *m_buttonText;
     QHBoxLayout *m_buttonLayout;
     QLabel *m_Iconlabel;

protected:

private Q_SLOTS:
     void webSearchTextRefresh(QString);

Q_SIGNALS:

};

#endif
