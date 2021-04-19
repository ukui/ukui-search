#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H
#include <QProxyStyle>
#include "global-settings.h"

class CustomStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit CustomStyle(const QString &proxyStyleName = "windows",QObject *parent = nullptr);
    virtual QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = nullptr) const;
};

#endif // CUSTOMSTYLE_H
