#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class UKuiSeachBar:public QWidget
{
  public:
    UKuiSeachBar();
    ~UKuiSeachBar();
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
};
