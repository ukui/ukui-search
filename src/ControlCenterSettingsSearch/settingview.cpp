#include "settingview.h"

settingview::settingview()
{
//    this->setStyleSheet("background:white;");
    this->setAttribute(Qt::WA_TranslucentBackground);//"透明背景"
}

settingview::~settingview()
{

}


void settingview::changesize(int row)
{
    qDebug()<<"changesize:"<<row;
    if(row<3){
        this->setFixedSize(Style::defaultMainViewWidWidth,row*28+30);
    } else {
        this->setFixedSize(Style::defaultMainViewWidWidth,3*28+30);
    }

}
