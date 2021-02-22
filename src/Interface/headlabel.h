#ifndef HEADLABEL_H
#define HEADLABEL_H

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
class HeadLabel : public QLabel
{
    Q_OBJECT
public:
    HeadLabel(QWidget *parent);
    void setText(QString text);

private:
    void InitUi();

private:
    QLabel *m_headText;

};

#endif // HEADLABEL_H
