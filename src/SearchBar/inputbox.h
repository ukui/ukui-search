#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

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
private:
    void initUI();
    void retouchLineEdit();

//    QLabel *m_queryIcon=nullptr;
//    QLabel *m_queryText=nullptr;
//    QLineEdit *m_queryLineEdit=nullptr;

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
public:
    UKuiSearchLineEdit();
    ~UKuiSearchLineEdit();
};
