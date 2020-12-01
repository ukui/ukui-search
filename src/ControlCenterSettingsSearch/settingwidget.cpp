#include "settingwidget.h"

SettingWidget::SettingWidget()
{
    initSettingsearchUI();

}

void SettingWidget::initSettingsearchUI()
{

    m_listLayout = new QVBoxLayout(this);
    settingHead = new QLabel(this);
    settingHead->setText("控制面板");

    settingView = new settingview;
    m_settingmodel = new settingModel;
    settingView->setModel(m_settingmodel);

    m_listLayout->addWidget(settingHead);
    m_listLayout->addWidget(settingView);
    this->setLayout(m_listLayout);
    this->setVisible(false);

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
    if(row<=0){
        this->setVisible(false);
    } else {
        this->setVisible(true);
    }
}
