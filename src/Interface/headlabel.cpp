#include "headlabel.h"

HeadLabel::HeadLabel(QWidget *parent)
{
     InitUi();
}

void HeadLabel::InitUi(){
    m_headText=new QLabel(this);
    this->setFixedHeight(35);
    QHBoxLayout *headLayout=new QHBoxLayout(this);
    headLayout->addSpacing(16);
    headLayout->addWidget(m_headText);
}

void HeadLabel::setText(QString text){
    m_headText->setText(text);
}
