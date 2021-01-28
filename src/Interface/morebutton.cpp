#include "morebutton.h"

MoreButton::MoreButton(QWidget *parent)
{
    InitUi();
}

void MoreButton::InitUi(){
    m_Textlabel=new QLabel;
    m_Iconlabel=new QLabel;
    m_Layout=new QHBoxLayout(this);
    m_Layout->addSpacing(16);
    m_Layout->addWidget(m_Textlabel);
    QColor color1 = palette().color(QPalette::Base);
    color1.setAlpha(0);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Button, QColor(color1));
    this->setPalette(pal);
    QPixmap pixmap(":/data/img/sidebarwidget/searching-more.svg");
    m_Iconlabel->setPixmap(pixmap);
    m_Layout->addWidget(m_Iconlabel,0,Qt::AlignRight);
}

void MoreButton::setText(QString text){
    m_Textlabel->setText(text);
}
