#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtDBus/QtDBus>

class UKuiSearchLineEdit;

class UKuiSeachBarWidget:public QWidget
{
public:
  UKuiSeachBarWidget();
  ~UKuiSeachBarWidget();
};

class UKuiSeachBar:public QWidget
{
  public:
    UKuiSeachBar();
    ~UKuiSeachBar();

private:
//    QLineEdit *m_queryLineEdit=nullptr;
};

class UkuiSearchBarHLayout : public QHBoxLayout
{
public:
    UkuiSearchBarHLayout();
    ~UkuiSearchBarHLayout();
    void searchContent(QString searchcontent);
private:
    void initUI();
    void retouchLineEdit();

//    QLabel *m_queryIcon=nullptr;
//    QLabel *m_queryText=nullptr;
//    QLineEdit *m_queryLineEdit=nullptr;
    UKuiSearchLineEdit *m_queryLineEdit=nullptr;

};
class UkuiSearchBarWidgetLayout : public QHBoxLayout
{
public:
    UkuiSearchBarWidgetLayout();
    ~UkuiSearchBarWidgetLayout();
private:
    void initUI();

};

class UKuiSearchLineEdit:public QLineEdit
{
    Q_OBJECT

    /*
     * 负责与ukui桌面环境应用通信的dbus
     * 搜索框文本改变的时候发送信号
　　　*/
    Q_CLASSINFO("D-Bus Interface", "org.ukui.search.inputbox")
public:
    UKuiSearchLineEdit();
    ~UKuiSearchLineEdit();

private Q_SLOTS:
    void  lineEditTextChanged(QString arg);
};
