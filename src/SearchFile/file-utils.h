#ifndef WIDGET_H
#define WIDGET_H

#include "gobject-template.h"
#include <QWidget>
#include <QIcon>

class FileUtils : public QWidget
{
    Q_OBJECT

public:
    FileUtils(QWidget *parent = nullptr);
    ~FileUtils();
    static QString getFileIconName(const QString &uri, bool checkValid = true);
};
#endif // WIDGET_H
