#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H
#include <QThread>
#include <QWaitCondition>
#include <QTimer>
#include "mainwindow.h"

class SearchResult : public QThread
{
    Q_OBJECT
public:
    SearchResult(QObject * parent = nullptr);
    ~SearchResult();
protected:
    void run() override;

private:
    MainWindow * m_mainwindow = nullptr;
//    QTimer * m_timer = nullptr;

Q_SIGNALS:
    void searchResultFile(const QString&);
    void searchResultDir(const QString&);
//    void searchResultContent(const QString&, const QStringList&);
    void searchResultContent(const QPair<QString, QStringList>);

};
#endif // SEARCHRESULT_H
