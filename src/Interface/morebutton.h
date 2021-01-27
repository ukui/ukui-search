#ifndef MOREBUTTON_H
#define MOREBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
class MoreButton : public QPushButton
{
    Q_OBJECT
public:
    MoreButton(QWidget *parent);
    void setText(QString text);

private:
    void InitUi();

private:
    QLabel *m_Textlabel;
    QLabel *m_Iconlabel;
    QHBoxLayout *m_Layout;
};
#endif // MOREBUTTON_H
