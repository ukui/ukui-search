#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include "control/search-list-view.h"

class ContentWidget : public QStackedWidget
{
    Q_OBJECT
public:
    ContentWidget(QWidget *);
    ~ContentWidget();

    void setPageType(const int&);
    int currentType();
private:
    void initUI();
    QWidget * m_homePage = nullptr;
    QVBoxLayout * m_homePageLyt = nullptr;
    QWidget * m_resultPage = nullptr;
    QHBoxLayout * m_resultPageLyt = nullptr;
    QScrollArea * m_resultListArea = nullptr;
    QScrollArea * m_resultDetailArea = nullptr;
    QWidget * m_resultList = nullptr;
    QVBoxLayout * m_listLyt = nullptr;
    QWidget * m_resultDetail = nullptr;
    QVBoxLayout * m_detailLyt = nullptr;

    int m_currentType = 0;

    QString getTitleName(const int&);

private Q_SLOTS:
    void refreshSearchList(const QVector<int>&, const QVector<QStringList>&);
    void clearSearchList();
};

#endif // CONTENTWIDGET_H
