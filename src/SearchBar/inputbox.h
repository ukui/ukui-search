#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtDBus/QtDBus>
#include <QPainter>
#include <QAction>
#include <QGSettings>
#include <QMouseEvent>


#define ORG_UKUI_STYLE            "org.ukui.style"
#define STYLE_NAME                "styleName"
#define STYLE_NAME_KEY_DARK       "ukui-dark"
#define STYLE_NAME_KEY_DEFAULT    "ukui-default"
#define STYLE_NAME_KEY_BLACK       "ukui-black"
#define STYLE_NAME_KEY_LIGHT       "ukui-light"
#define STYLE_NAME_KEY_WHITE       "ukui-white"

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

    //控制style的gsetting
    QGSettings *style_settings;
    QGSettings *model_gsettings;
    //用来判断style
    QStringList stylelist;
    void styleChange();
    QLabel *editLabel;

private:
    bool mModel;
protected:
        void mousePressEvent(QMouseEvent *event);

private:
        void sogouInput();

private Q_SLOTS:
    void  lineEditTextChanged(QString arg);
};
