#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QGSettings>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QSettings>
#include <QFileDialog>
#include <QTextCodec>
#include <QPushButton>
#include <QMessageBox>

#include <ukcc/interface/interface.h>
#include <ukcc/widgets/comboboxitem.h>
#include <ukcc/widgets/switchbutton.h>
#include <ukcc/widgets/hoverwidget.h>
#include <ukcc/widgets/switchbutton.h>
#include <ukcc/widgets/comboxframe.h>
#include <ukcc/widgets/titlelabel.h>

#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define SEARCH_METHOD_KEY "indexSearch"
#define WEB_ENGINE_KEY "webEngine"
//TODO
#define CONFIG_FILE "/.config/org.ukui/ukui-search/ukui-search-block-dirs.conf"

enum ReturnCode {
    Succeed,
    PathEmpty,
    NotInHomeDir,
    ParentExist,
    HasBeenBlocked
};

class Search : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ukcc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    explicit Search();
    ~Search();

    QString plugini18nName()   Q_DECL_OVERRIDE;
    int pluginTypes()       Q_DECL_OVERRIDE;
    QWidget * pluginUi()   Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;
    QString translationPath() const;
    bool isShowOnHomePage() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    bool isEnable() const Q_DECL_OVERRIDE;

private:
    QWidget * m_pluginWidget = nullptr;
    QString m_plugin_name = "";
    int m_plugin_type = 0;

    QGSettings * m_gsettings = nullptr;

    void initUi();
    QVBoxLayout * m_mainLyt = nullptr;
    TitleLabel * m_titleLabel = nullptr;

    QFrame *m_setFrame = nullptr;
    QVBoxLayout *m_setFrameLyt = nullptr;
    //设置搜索模式
    QFrame *m_descFrame = nullptr;
    QVBoxLayout *m_descFrameLyt = nullptr;
    QLabel *m_descLabel1 = nullptr;
    QLabel *m_descLabel2 = nullptr;
    QFrame *m_searchMethodFrame = nullptr;
    QHBoxLayout *m_searchMethodLyt = nullptr;
//    QLabel *m_searchMethodLabel = nullptr;
    SwitchButton * m_searchMethodBtn = nullptr;
    //设置搜索引擎
    TitleLabel * m_webEngineLabel = nullptr;
    ComboxFrame * m_webEngineFrame = nullptr;
    QVBoxLayout * m_webEngineLyt = nullptr;

    //设置黑名单
    TitleLabel * m_blockDirTitleLabel = nullptr;
    QLabel * m_blockDirDescLabel = nullptr;
    QFrame * m_blockDirsFrame = nullptr;
    QVBoxLayout * m_blockDirsLyt = nullptr;
    QPushButton * m_addBlockDirWidget = nullptr;
    QLabel * m_addBlockDirIcon = nullptr;
    QLabel * m_addBlockDirLabel = nullptr;
    QHBoxLayout * m_addBlockDirLyt = nullptr;

    QStringList m_blockDirs;
    QSettings * m_dirSettings = nullptr;
    void getBlockDirs();
    int setBlockDir(const QString &dirPath, const bool &is_add = true);
    void appendBlockDirToList(const QString &path);
    void removeBlockDirFromList(const QString &path);
    void initBlockDirsList();
//    void refreshBlockDirsList();


    void setupConnection();

private slots:
    void onBtnAddFolderClicked();
};

#endif // SEARCH_H
