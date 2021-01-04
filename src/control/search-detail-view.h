#ifndef SEARCHDETAILVIEW_H
#define SEARCHDETAILVIEW_H

#include <QWidget>
#include "option-view.h"

class SearchDetailView : public QWidget
{
    Q_OBJECT
public:
    explicit SearchDetailView(QWidget *parent = nullptr);
    ~SearchDetailView();

    void setupWidget(const int&, const QString&);
    void clearLayout();
    void setContent(const QString&);

private:
    QVBoxLayout * m_layout = nullptr;
    QString m_contentText;
    bool openAction(const int&, const QString&);
    bool addDesktopShortcut(const QString&);
    bool addPanelShortcut(const QString&);
    bool openPathAction(const QString&);
    bool copyPathAction(const QString&);
Q_SIGNALS:

private Q_SLOTS:
    void execActions(const int&, const int&, const QString&);
};

#endif // SEARCHDETAILVIEW_H
