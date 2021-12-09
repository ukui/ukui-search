#ifndef SEPARATIONLINE_H
#define SEPARATIONLINE_H

#include <QObject>
#include <QFrame>

class SeparationLine : public QFrame
{
    Q_OBJECT
public:
    SeparationLine(QWidget *parent = nullptr);
    ~SeparationLine() = default;
public Q_SLOTS:
    void setLineStyle();
};

#endif // SEPARATIONLINE_H
