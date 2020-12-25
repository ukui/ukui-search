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
    explicit OptionView(QWidget *, const int&);
    ~OptionView();

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
    void initComponent(const int&);
    void setupAppOptions();
    void setupFileOptions();
    void setupDirOptions();
    void setupSettingOptions();
    void setupOptionLabel(const int&);

    int m_type;

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
