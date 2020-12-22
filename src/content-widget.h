#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

class ContentWidget : public QStackedWidget
{
    Q_OBJECT
public:
    ContentWidget(QWidget * parent);
    ~ContentWidget();

    enum SearchType {
        All,
        Apps,
        Settings,
        Files
    };

    void setPageType(int type);
    int currentType();
private:
    void initUI();
    QWidget * homePage = nullptr;
    QVBoxLayout * homePageLyt = nullptr;
    QWidget * resultPage = nullptr;
    QHBoxLayout * resultPageLyt = nullptr;
    QScrollArea * resultListArea = nullptr;
    QScrollArea * resultDetailArea = nullptr;
    QWidget * resultList = nullptr;
    QWidget * resultDetail = nullptr;

    int m_current_type = 0;
};

#endif // CONTENTWIDGET_H
