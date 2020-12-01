#include "fileview.h"

fileview::fileview()
{
//    this->setStyleSheet("background:transparent;");
    this->setAttribute(Qt::WA_TranslucentBackground);//"透明背景"
    this->setHeaderHidden(true);
}
