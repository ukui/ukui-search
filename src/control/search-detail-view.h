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
    void setContent(const QString&, const QString&);
    bool isEmpty();
    int getType();

private:
    QVBoxLayout * m_layout = nullptr;
    QString m_contentText;
    QString m_keyword;
    bool openAction(const int&, const QString&);
    bool addDesktopShortcut(const QString&);
    bool addPanelShortcut(const QString&);
    bool openPathAction(const QString&);
    bool copyPathAction(const QString&);
    QString getHtmlText(const QString&, const QString&);
    bool writeConfigFile(const QString&);
    bool m_isEmpty = true;
    int m_type = 0;
    QString m_path = 0;

    void initUI();
    QLabel * m_iconLabel = nullptr;
    QFrame * m_nameFrame = nullptr;
    QHBoxLayout * m_nameLayout = nullptr;
    QLabel * m_nameLabel = nullptr;
    QLabel * m_typeLabel = nullptr;
    QFrame * m_hLine = nullptr;
    QFrame * m_detailFrame = nullptr;
    QVBoxLayout * m_detailLyt = nullptr;
    QLabel * m_contentLabel = nullptr;
    QFrame * m_pathFrame = nullptr;
    QFrame * m_timeFrame = nullptr;
    QHBoxLayout * m_pathLyt = nullptr;
    QHBoxLayout * m_timeLyt = nullptr;
    QLabel * m_pathLabel_1 = nullptr;
    QLabel * m_pathLabel_2 = nullptr;
    QLabel * m_timeLabel_1 = nullptr;
    QLabel * m_timeLabel_2 = nullptr;
    QFrame * m_hLine_2 = nullptr;
    OptionView * m_optionView = nullptr;

Q_SIGNALS:
    void configFileChanged();
private Q_SLOTS:
    void execActions(const int&, const int&, const QString&);
};

#endif // SEARCHDETAILVIEW_H
