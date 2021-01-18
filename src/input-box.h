#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtDBus/QtDBus>
#include <QPainter>
#include <QAction>
#include <QTimer>

class SearchLineEdit;

class SeachBarWidget:public QWidget
{
public:
  SeachBarWidget();
  ~SeachBarWidget();
};

class SeachBar:public QWidget
{
  public:
    SeachBar();
    ~SeachBar();

private:
//    QLineEdit *m_queryLineEdit=nullptr;
};

class SearchBarHLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    SearchBarHLayout();
    ~SearchBarHLayout();
    void clearText();
    QString text();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void initUI();
    bool m_isEmpty = true;
    QTimer * m_timer = nullptr;

    SearchLineEdit * m_queryLineEdit = nullptr;
    QPropertyAnimation * m_animation = nullptr;
    QWidget * m_queryWidget = nullptr;
    QLabel * m_queryIcon = nullptr;
    QLabel * m_queryText = nullptr;
    bool m_isSearching = false;

Q_SIGNALS:
    void textChanged(QString text);

};
class SearchBarWidgetLayout : public QHBoxLayout
{
public:
    SearchBarWidgetLayout();
    ~SearchBarWidgetLayout();
private:
    void initUI();

};

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

    /*
     * 负责与ukui桌面环境应用通信的dbus
     * 搜索框文本改变的时候发送信号
　　　*/
    Q_CLASSINFO("D-Bus Interface", "org.ukui.search.inputbox")
public:
    SearchLineEdit();
    ~SearchLineEdit();

private Q_SLOTS:
    void  lineEditTextChanged(QString arg);
};
