#ifndef SEARCHLINEEDITVIEW_H
#define SEARCHLINEEDITVIEW_H

#include <QObject>
#include <QQuickView>
#include <QDesktopWidget>

class SearchLineEditView : public QQuickView
{
    Q_OBJECT
public:
    SearchLineEditView();
    void showWindow();

public Q_SLOTS:
    void onTextChange(QString text);
Q_SIGNALS:
    void textChange(QString);

private:
    QDesktopWidget *m_desktopWidget;

};

#endif // SEARCHLINEEDITVIEW_H
