#include "custom-style.h"

CustomStyle::CustomStyle(const QString &proxyStyleName, QObject *parent) : QProxyStyle(proxyStyleName) {

}

QSize CustomStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const {
    switch(type) {
    case CT_ItemViewItem: {
        QSize size(0, GlobalSettings::getInstance()->getValue(FONT_SIZE_KEY).toDouble() * 2);
        return size;
    }
    break;
    default:
        break;
    }
    return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
}
