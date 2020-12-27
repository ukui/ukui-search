#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtDBus/QtDBus>
#include <QPainter>
#include <QAction>
#include <QTimer>

class UKuiSearchLineEdit;

class UKuiSeachBarWidget:public QWidget
{
public:
  UKuiSeachBarWidget();
  ~UKuiSeachBarWidget();
  void paintEvent(QPaintEvent *e);
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
    Q_OBJECT
public:
    UkuiSearchBarHLayout();
    ~UkuiSearchBarHLayout();
    void searchContent(QString searchcontent);
    void clearText();
private:
    void initUI();
    bool m_isEmpty = true;
    QTimer * m_timer = nullptr;

    UKuiSearchLineEdit *m_queryLineEdit=nullptr;

Q_SIGNALS:
    void textChanged(QString text);

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
