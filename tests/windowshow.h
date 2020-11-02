#ifndef WINDOWSHOW_H
#define WINDOWSHOW_H

#include <QObject>
#include <QWidget>
#include <QStandardItemModel>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>


class windowshow : public QObject,QWidget
{
    Q_OBJECT
public:
    explicit windowshow(QObject *parent = nullptr);


};

#endif // WINDOWSHOW_H
