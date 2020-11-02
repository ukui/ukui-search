#include <QApplication>
#include "mainrunner.h"
#include "pinyin.h"
#include "filtermodel.h"

mainrunner::mainrunner(QWidget *parent)

{
    QDesktopWidget* desktop = QApplication::desktop();
    m_bIstextEdited=false;
    m_areaMovable = geometry();
    m_bPressed = false;
    search1 = QString::fromLocal8Bit("");

    RunnerModel *runnerModel = new RunnerModel(this);
    m_pMainVLayout = new QVBoxLayout(this);//搜索总布局
    m_pMainVLayout->setContentsMargins(0, 0, 0, 0);
    m_pMainVLayout->setSpacing(0);

    input = new QLineEdit(this); //输入框

    //设置搜索框样式
    QAction *searchAction = new QAction(input);
    searchAction->setIcon(QIcon(QString::fromLocal8Bit(":/search.png")));
    input->addAction(searchAction,QLineEdit::LeadingPosition);
    input->setStyleSheet(QString::fromLocal8Bit("QLineEdit { background-color : white; border:1px solid rgb(0, 0, 222);border-radius: 10px;}"));

//   //设置控制面板模块搜索栏
//    m_searchWidget =new SearchWidget(this);
//    m_searchWidget->addModulesName(QString::fromLocal8Bit("update"),QString::fromLocal8Bit("更新"),QString::fromLocal8Bit("/home/li/ukui/ukui-runner/ukui-runner/tests/zh_CN.ts"));
//    m_searchWidget->setLanguage(QString::fromLocal8Bit("1"));

    connect(input, &QLineEdit::textChanged, runnerModel, &RunnerModel::scheduleQuery); //监听输入框输入的信息

    //初始化搜索载体
    fileView = new QTreeView(this);//文档搜索treeview
    appView  = new QTreeView(this);//应用程序搜索treeview
    setView  = new QTreeView(this);//设置搜索treeview
    Search_web_pages = new QPushButton();//网页搜索QPushButton
    Search_web_pages->setText(QString::fromLocal8Bit("网页搜索"));



    connect(input, &QLineEdit::textChanged, Search_web_pages,[=](){
        QString search=QString::fromLocal8Bit("使用百度搜索").append(QString::fromLocal8Bit(" ")).append(QString::fromLocal8Bit("\"")).append(input->text()).append(QString::fromLocal8Bit("\""));
//        QString search = QString::fromLocal8Bit(QString("使用百度搜索 1%2%3%").arg(QString::fromLocal8Bit("")).arg(input->text().arg(QString::fromLocal8Bit(""))));
        Search_web_pages->setText(search);
        search1=input->text();
        //根据判断来隐藏与显示网页搜索
        if(search1 != QString::fromLocal8Bit("")){
            Search_web_pages->setVisible(true);
        } else {
            Search_web_pages->setVisible(false);
        }
    }); //监听输入框输入的信息并把信息发送给搜索网页按钮

    fileView->setStyleSheet(QString::fromLocal8Bit("QTreeView { background-color : white; border: 0px none;border-radius: 10px;}")) ;
    appView->setStyleSheet(QString::fromLocal8Bit("QTreeView { background-color : white; border: 0px none;border-radius: 10px;}")) ;
    setView->setStyleSheet(QString::fromLocal8Bit("QTreeView { background-color : white; border: 0px none;border-radius: 10px;}")) ;
    Search_web_pages->setStyleSheet(QString::fromLocal8Bit("text-align:left"));



//    sliderP1->setStyleSheet(

//                "QSlider::groove:Vertical {"
//                 "border: 0px none;"
//                 "background: #F5F5F5;"
//                 "height: 222px;"
//                 "width: 32px;"
//                 "border-radius: 5px;"
//                 "}"

//                 "QSlider::handle:Vertical {"
//                 "background: #00BFFF;"
//                 "border: 0px none;"
//                 "width: 50px;"
//                 "height: 15px;"
//                 "border-radius: 5px;"
//                 "margin: 0px 0;"
//                 "border-image:url(:/image/abcd.png);"
//                 "}"

//                 "QSlider::add-page:Vertical {"
//                 "background: #00BFFF;"
//                 "border-radius: 5px;"
//                 "}"

//                 "QSlider::sub-page:horizontal {"
//                 "background: #DD4814;"
//                 "border-radius: 5px;"
//                 "}"

//             );

    //添加搜索依据内容
    filefiltermodel *mFileModel = new filefiltermodel;
    mFileModel->setSourceModel(runnerModel);
    fileView->setModel(mFileModel);

    appfiltermodel *mAppModel = new appfiltermodel;
    mAppModel->setSourceModel(runnerModel);
    appView->setModel(mAppModel);

    model = new QStandardItemModel();
    setView->setModel(model);

    //搜索控制面板设置
    mqsetting =new QSettings(QString::fromLocal8Bit("/home/li/Desktop/search.conf"),QSettings::IniFormat);
    mqsetting->setIniCodec(QTextCodec::codecForName("UTF-8")); //在此添加设置，即可读写ini文件中的中文

    connect(input, &QLineEdit::textChanged,[=](){
          listenchange();

          qDebug()<< mFileModel->index(0,0);

    }); //监听输入框输入的信息并把信息发送给系统设置界面


    //整体布局
    m_pMainVLayout->addWidget(input);
    m_pMainVLayout->addItem(new QSpacerItem(20, 10));
    m_pMainVLayout->addWidget(fileView);
    m_pMainVLayout->addItem(new QSpacerItem(20, 10));
    m_pMainVLayout->addWidget(appView);
    m_pMainVLayout->addItem(new QSpacerItem(20, 10));
    m_pMainVLayout->addWidget(setView);
    m_pMainVLayout->addItem(new QSpacerItem(20, 10));
    m_pMainVLayout->addWidget(Search_web_pages);
//    m_pMainVLayout->addWidget(m_searchWidget);


//    fileView->setVisible(false);
//    appView->setVisible(false);
    setView->setVisible(false);
    Search_web_pages->setVisible(false);


    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setAttribute(Qt::WA_TranslucentBackground);//背景透明


    //根据点击事件打开应用、文件、控制面板、网页设置
    connect(appView,&QTreeView::clicked,this,[=](){
        runnerModel->run(appView->currentIndex().row());
    });

    connect(fileView,&QTreeView::clicked,this,[=](){
        runnerModel->run(mFileModel->mapToSource(fileView->currentIndex()).row());
    });

    connect(setView,&QTreeView::clicked,this,[=](){
        QProcess p;
        p.setProgram(search_command.at(setView->currentIndex().row()));
        p.startDetached(p.program());
        p.waitForFinished(-1);

    });

    connect(Search_web_pages,&QPushButton::clicked,this,[=](){
        QString str = QString::fromLocal8Bit("https://www.baidu.com/baidu?tn=ubuntuu_cb&ie=utf-8&wd=").append(search1);
        QProcess p;
        p.setProgram(QString::fromLocal8Bit("firefox"));
        p.setArguments(QStringList()<<str);
        p.startDetached(p.program(), p.arguments());
        p.waitForFinished(-1);
    });

//    qDebug()<<desktop->width() - this->width()/2<<  (desktop->height() - this->height())/2;
//    this->move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

}

mainrunner::~mainrunner()
{

}

void mainrunner::listenchange()
{
    //清空model数据
    model->clear();
    search_command.clear();
    //打开配置文件
    mqsetting->beginGroup(QString::fromLocal8Bit("contol"));
    searchconter=QString::fromLocal8Bit("");

//        qDebug()<<mqsetting->allKeys();
//        qDebug()<<mqsetting->value(QString::fromLocal8Bit("kongzhizhongxin")).toString();

    QString inputget=Chinese2Pinyin(input->text());
    qDebug()<<inputget;
    QStringList valuenum=mqsetting->allKeys();

    //判断字符串匹配
    for(int i=0;i<valuenum.count();i++){
        if(valuenum.at(i) == inputget){
             qDebug()<<"okokokok";
             searchconter=valuenum.at(i);
        }

    }
    //通过判断字符串是否为空，来设置model
    qDebug()<<searchconter;
    if(searchconter != QString::fromLocal8Bit("")){
      search_command.append(mqsetting->value(searchconter).toStringList());
      qDebug()<<search_command.at(1);
      QStandardItem* itemChild = new QStandardItem(search_command.at(1));
      model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("控制面板"));
      model->appendRow(itemChild);
      setView->setModel(model);
      setView->setVisible(true);

    } else {
      setView->setVisible(false);
    }

   mqsetting->endGroup();

}

void mainrunner::mousePressEvent(QMouseEvent *e)
{
  //鼠标左键
  if(e->button() == Qt::LeftButton)
  {
  m_ptPress = e->pos();
  qDebug() << pos() << e->pos() << m_ptPress;
  m_bPressed = m_areaMovable.contains(m_ptPress);
  }
}

void mainrunner::mouseMoveEvent(QMouseEvent *e)
{
  if(m_bPressed)
  {
  qDebug() << pos() << e->pos() << m_ptPress;
  move(pos() + e->pos() - m_ptPress);
  }
}

void mainrunner::mouseReleaseEvent(QMouseEvent *)
{
  m_bPressed = false;
}


void mainrunner::setAreaMovable(const QRect rt)
{
  if(m_areaMovable != rt)
  {
  m_areaMovable = rt;
  }
}



//QString mainrunner::transPinyinToChinese(QString pinyin) {
//    QString value = pinyin;

//    //遍历"汉字-拼音"列表,将存在的"拼音"转换为"汉字"
//    for (auto data : m_inputList) {
//        if (value == data.pinyin) {
//            value = data.chiese;
//            break;
//        }
//    }

//    return value;
//}

//QString mainrunner::containTxtData(QString txt) {
//    QString value = txt;

//    //遍历"汉字-拼音"列表,将存在的"拼音"转换为"汉字"
//    for (auto data : m_inputList) {
//        if (data.chiese.contains(txt, Qt::CaseInsensitive) ||
//               data.pinyin.contains(txt, Qt::CaseInsensitive)) {
//            value = data.chiese;
//            break;
//        }
//    }

//    return value;
//}

