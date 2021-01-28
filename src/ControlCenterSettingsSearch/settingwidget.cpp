#include "settingwidget.h"

SettingWidget::SettingWidget()
{
    initSettingsearchUI();
    installEventFilter(this);
}

void SettingWidget::initSettingsearchUI()
{

    m_listLayout = new QVBoxLayout(this);
    m_Headlabel = new HeadLabel(this);
    m_Headlabel->setText(tr("Settings"));



    settingView = new settingview;
    m_settingmodel = new settingModel;
    settingView->setModel(m_settingmodel);

    m_Button=new MoreButton(this);
    m_Button->setText("在系统设置中搜索更多内容");

    m_listLayout->addWidget(m_Headlabel);
    m_listLayout->addWidget(settingView);
    m_listLayout->addWidget(m_Button);
    this->setLayout(m_listLayout);
    this->setVisible(false);

    connect(m_Button,&MoreButton::clicked,this,[=](){
        QProcess *process =new QProcess(this);
        process->startDetached("ukui-control-center");
    });

    //监听点击事件，打开对应的设置选项
    connect(settingView,&QTreeView::clicked,this,[=](){
        m_settingmodel->run(settingView->currentIndex().row());
    });


    //监听搜索出的设置数量，做界面更改
    connect(m_settingmodel,&settingModel::requestUpdateSignal,this,&SettingWidget::recvSettingSearchResult);

    //监听输入框的改变，刷新界面
    QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(settingTextRefresh(QString)));


}

void SettingWidget::settingTextRefresh(QString mSearchText)
{
    m_settingmodel->matchstart(mSearchText);
}

void SettingWidget::recvSettingSearchResult(int row)
{
    if(row>2){
        this->setFixedHeight(m_Headlabel->height()+(row+1.7)*46);
        m_Button->show();
    }else{
        this->setFixedHeight(m_Headlabel->height()+(row+1)*46);
        m_Button->hide();
    }
    if(row<=0){
        this->setVisible(false);
    } else {
        this->setVisible(true);
    }
}


void SettingWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(qApp->palette().color(QPalette::Base));
//    p.setBrush(QBrush(QColor(255,255,255)));
    p.setOpacity(1);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,12,12);
    QWidget::paintEvent(e);
}

bool SettingWidget::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::MouseButtonRelease){
            return true;
        }
    }
    return false;
}
