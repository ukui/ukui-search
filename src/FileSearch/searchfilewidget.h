#ifndef SEARCHFILEWIDGET_H
#define SEARCHFILEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QtDBus/QtDBus>
#include <QDebug>
#include <QString>
#include <QPainter>
#include "fileview.h"
#include "filemodel.h"
#include "searchfilethread.h"
#include "../Interface/morebutton.h"
#include "../Interface/headlabel.h"

class SearchFileWidget : public QWidget
{
    Q_OBJECT
public:
     SearchFileWidget();
     fileview *fileView;
     filemodel *m_filemodel;//文件model
     QVBoxLayout *m_listLayout;
     SearchFileThread *m_searchFileThread=nullptr;
private:
     void initFilesearchUI();
     void paintEvent(QPaintEvent *e);
     bool eventFilter(QObject *watched, QEvent *event);

private:
     MoreButton *m_Morebutton;
     HeadLabel *m_fileHead;

private Q_SLOTS:
     void fileTextRefresh(QString);
     void recvFileSearchResult(QStringList arg);

Q_SIGNALS:
     void sendSearchKeyword(QString arg);


};

#endif // SEARCHFILEWIDGET_H
