#ifndef TITLELABEL_H
#define TITLELABEL_H

#include <QLabel>

class TitleLabel : public QLabel
{
public:
    TitleLabel(QWidget * parent = nullptr);
    ~TitleLabel();

protected:
    void paintEvent(QPaintEvent *);
};

#endif // TITLELABEL_H
