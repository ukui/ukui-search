#include "websearch.h"

websearch::websearch()
{
    this->setStyleSheet("QPushButton{text-align: left;}");
    this->setFixedHeight(40);
    this->setAttribute(Qt::WA_TranslucentBackground);//"透明背景"

    //监听输入框的改变，刷新界面
    QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(webSearchTextRefresh(QString)));
}

/**
 * @brief 监听信号刷新网页搜索的文字
 * @param mSearchText
 */
void websearch::webSearchTextRefresh(QString mSearchText)
{

    this->setText(mSearchText);
}
