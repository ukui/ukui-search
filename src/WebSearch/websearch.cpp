#include "websearch.h"

websearch::websearch()
{
    this->setStyleSheet("QPushButton{text-align: left;}");
    this->setText("   请使用百度搜索");
    this->setFixedHeight(40);
    this->setAttribute(Qt::WA_TranslucentBackground);//"透明背景"
}
