#ifndef MAINRUNNER_H
#define MAINRUNNER_H

#include <QWidget>
#include <QString>
#include <QStandardItemModel>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTreeView>
#include <QLineEdit>
#include <QDebug>
#include "declarative/runnermodel.h"
#include <QMouseEvent>
#include "filtermodel.h"
#include <QPushButton>
#include <QProcess>
#include "searchwidget.h"
#include <QPropertyAnimation>
#include <QProcess>
#include <QTextCodec>
#include <QComboBox>
#include <QDesktopWidget>
#include <QAction>
#include <QStyleFactory>


#include <QSettings>
class mainrunner : public QWidget
{
    Q_OBJECT

public:
    mainrunner(QWidget *parent = nullptr);
    ~mainrunner();

    void setAreaMovable(const QRect rt);

    struct SearchDataStruct {
        QString chiese;
        QString pinyin;
    };


private:

      QVBoxLayout  *m_pMainVLayout    = nullptr;

      QTreeView *fileView;
      QTreeView *appView;
      QTreeView *setView;
      QPushButton *Search_web_pages;
      QString search1;
      QLineEdit *input;

      QRect     m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
      bool      m_bPressed;//鼠标按下标志（不分左右键）
      QPoint    m_ptPress;//鼠标按下的初始位置



      void mousePressEvent(QMouseEvent *);
      void mouseMoveEvent(QMouseEvent *);
      void mouseReleaseEvent(QMouseEvent *);

      void listenchange();


      //测试conf文件

      bool m_bIstextEdited;
      QSettings *mqsetting;

      QString searchconter;
      QString m_searchValue;

      QStandardItemModel* model;
//      QString  transPinyinToChinese(QString pinyin);
//      QString  containTxtData(QString txt);
      QList<SearchDataStruct> m_inputList;
      QWidget           *m_queryWid=nullptr;
      SearchWidget      * m_searchWidget;
      QLabel            *m_queryIcon;
      QLabel            *m_queryText=nullptr;

      QPropertyAnimation *m_animation=nullptr;

      QList<QString> search_command;

      QComboBox *combox;

};
#endif // MAINRUNNER_H
