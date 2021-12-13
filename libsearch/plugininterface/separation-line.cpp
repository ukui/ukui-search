#include "separation-line.h"
#include "global-settings.h"

#define NOMORL_LINE_STYLE "QFrame{background: rgba(0,0,0,0.1);}"
#define DARK_LINE_STYLE "QFrame{background: rgba(255, 255, 255, 0.16);}"

using namespace UkuiSearch;
SeparationLine::SeparationLine(QWidget *parent) : QFrame(parent)
{
    this->setLineWidth(0);
    this->setFixedHeight(1);
    setLineStyle();
    connect(qApp, &QApplication::paletteChanged, this, &SeparationLine::setLineStyle);
}

void SeparationLine::setLineStyle()
{
    QString type = GlobalSettings::getInstance()->getValue(STYLE_NAME_KEY).toString();
    if (type == "ukui-dark") {
        this->setStyleSheet(DARK_LINE_STYLE);
    } else {
        this->setStyleSheet(NOMORL_LINE_STYLE);
    }
}
