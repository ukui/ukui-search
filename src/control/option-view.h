#ifndef OPTIONVIEW_H
#define OPTIONVIEW_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "search-list-view.h"

class OptionView : public QWidget
{
    Q_OBJECT
public:
    explicit OptionView(QWidget *);
    ~OptionView();
    void setupOptions(const int&);

    enum Options {
        Open,
        Shortcut,
        Panel,
        OpenPath,
        CopyPath
    };

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    void initUI();
    void setupAppOptions();
    void setupFileOptions();
    void setupDirOptions();
    void setupSettingOptions();
    void setupOptionLabel(const int&);
    void hideOptions();

    int m_type;

    QFrame * m_optionFrame = nullptr;
    QVBoxLayout * m_optionLyt = nullptr;
    QVBoxLayout * m_mainLyt = nullptr;
    QLabel * m_openLabel = nullptr;
    QLabel * m_shortcutLabel = nullptr;
    QLabel * m_panelLabel  = nullptr;
    QLabel * m_openPathLabel = nullptr;
    QLabel * m_copyPathLabel = nullptr;

Q_SIGNALS:
    void onOptionClicked(const int&);
};

#endif // OPTIONVIEW_H
