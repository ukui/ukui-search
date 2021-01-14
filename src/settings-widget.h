#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

class SettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void setIndexState(bool);
    void setIndexNum(int);
    void showWidget();

private:
    void initUi();
    void setupBlackList(const QStringList &);
    void clearLayout(QLayout *);
    void refreshIndexState();
    void paintEvent(QPaintEvent *);
    //标题栏
    QVBoxLayout * m_mainLyt = nullptr;
    QFrame * m_titleFrame = nullptr;
    QHBoxLayout * m_titleLyt = nullptr;
    QLabel * m_titleIcon = nullptr;
    QLabel * m_titleLabel = nullptr;
    QPushButton * m_closeBtn = nullptr;

    //设置
    QLabel * m_settingLabel = nullptr;
    //文件索引
    QLabel * m_indexTitleLabel = nullptr;
    QLabel * m_indexStateLabel = nullptr;
    QLabel * m_indexNumLabel = nullptr;
    //文件索引设置（黑名单）
    QLabel * m_indexSettingLabel = nullptr;
    QLabel * m_indexDescLabel = nullptr;
    QFrame * m_indexBtnFrame = nullptr;
    QHBoxLayout * m_indexBtnLyt = nullptr;
    QPushButton * m_addDirBtn = nullptr;
    QScrollArea * m_dirListArea = nullptr;
    QWidget * m_dirListWidget = nullptr;
    QVBoxLayout * m_dirListLyt = nullptr;

    //搜索引擎设置
    QLabel * m_searchEngineLabel = nullptr;
    QLabel * m_engineDescLabel = nullptr;
    QButtonGroup * m_engineBtnGroup = nullptr;
    QFrame * m_radioBtnFrame = nullptr;
    QHBoxLayout * m_radioBtnLyt = nullptr;
    QRadioButton * m_baiduBtn = nullptr;
    QLabel * m_baiduLabel = nullptr;
    QRadioButton * m_sougouBtn = nullptr;
    QLabel * m_sougouLabel = nullptr;
    QRadioButton * m_360Btn = nullptr;
    QLabel * m_360Label = nullptr;

    //取消与确认按钮
    QFrame * m_bottomBtnFrame = nullptr;
    QHBoxLayout * m_bottomBtnLyt = nullptr;
    QPushButton * m_cancelBtn = nullptr;
    QPushButton * m_confirmBtn = nullptr;

    QTimer * m_timer;

Q_SIGNALS:
    void settingWidgetClosed();

private Q_SLOTS:
    void onBtnConfirmClicked();
    void onBtnCancelClicked();
    void onBtnAddClicked();
    void onBtnDelClicked(const QString&);
};

#endif // SETTINGSWIDGET_H
