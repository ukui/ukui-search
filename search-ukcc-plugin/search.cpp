#include "search.h"
#include <QTranslator>
#include <QApplication>

Search::Search()
{
    QTranslator* translator = new QTranslator(this);
    if(!translator->load("/usr/share/ukui-search/search-ukcc-plugin/translations/" + QLocale::system().name())) {
        qWarning() << "/usr/share/ukui-search/search-ukcc-plugin/translations/" + QLocale::system().name() << "load failed";
    }
    QApplication::installTranslator(translator);

    m_plugin_name = tr("Search");
    m_plugin_type = SEARCH_F;
    initUi();
    setupConnection();
    m_dirSettings = new QSettings(QDir::homePath() + CONFIG_FILE, QSettings::NativeFormat, this);
    m_dirSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    initBlockDirsList();
}

QString Search::plugini18nName()
{
    return m_plugin_name;
}

int Search::pluginTypes()
{
    return m_plugin_type;
}

QWidget *Search::pluginUi()
{
    m_pluginWidget->setAttribute(Qt::WA_DeleteOnClose);

    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if (QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id, QByteArray(), this);
        //按钮状态初始化
        if (m_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            //当前是否使用索引搜索/暴力搜索
            bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
            m_searchMethodBtn->setChecked(is_index_search_on);
        } else {
            m_searchMethodBtn->setEnabled(false);
        }
        if (m_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            //当前网页搜索的搜索引擎
            QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
            m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
        } else {
            m_webEngineFrame->mCombox->setEnabled(false);
        }
        //监听gsettings值改变，更新控制面板UI
        connect(m_gsettings, &QGSettings::changed, this, [ = ](const QString &key) {
            if (key == SEARCH_METHOD_KEY) {
                bool is_index_search_on = m_gsettings->get(SEARCH_METHOD_KEY).toBool();
                m_searchMethodBtn->blockSignals(true);
                m_searchMethodBtn->setChecked(is_index_search_on);
                m_searchMethodBtn->blockSignals(false);
            } else if (key == WEB_ENGINE_KEY) {
                QString engine = m_gsettings->get(WEB_ENGINE_KEY).toString();
                m_webEngineFrame->mCombox->blockSignals(true);
                m_webEngineFrame->mCombox->setCurrentIndex(m_webEngineFrame->mCombox->findData(engine));
                m_webEngineFrame->mCombox->blockSignals(false);
            }
        });
        connect(m_searchMethodBtn, &kdk::KSwitchButton::stateChanged, this, [ = ](bool checked) {
            if (m_gsettings && m_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
                m_gsettings->set(SEARCH_METHOD_KEY, checked);
            }
        });
        connect(m_webEngineFrame->mCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
            if (m_gsettings && m_gsettings->keys().contains(WEB_ENGINE_KEY)) {
                m_gsettings->set(WEB_ENGINE_KEY, m_webEngineFrame->mCombox->currentData().toString());
            }
        });
    } else {
        qCritical() << UKUI_SEARCH_SCHEMAS << " not installed!\n";
        m_searchMethodBtn->setEnabled(false);
        m_webEngineFrame->mCombox->setEnabled(false);
    }
    return m_pluginWidget;
}

const QString Search::name() const
{
    return QStringLiteral("Search");
}

QString Search::translationPath() const
{
    return QStringLiteral("/usr/share/ukui-search/search-ukcc-plugin/translations/%1.ts");
}

bool Search::isShowOnHomePage() const
{
    return true;
}

QIcon Search::icon() const
{
    return QIcon::fromTheme("search-symbolic");
}

bool Search::isEnable() const
{
    return true;
}

/**
 * @brief Search::initUi 初始化此插件UI
 */
void Search::initUi()
{
    m_pluginWidget = new QWidget;
    m_mainLyt = new QVBoxLayout(m_pluginWidget);
    m_pluginWidget->setLayout(m_mainLyt);

    m_titleLabel = new TitleLabel(m_setFrame);
    m_titleLabel->setText(tr("Search"));
    m_mainLyt->addWidget(m_titleLabel);

    //设置搜索模式部分的ui
    m_setFrame = new QFrame(m_pluginWidget);
    m_setFrame->setFrameShape(QFrame::Shape::Box);
    m_setFrameLyt = new QVBoxLayout(m_setFrame);
    m_setFrameLyt->setContentsMargins(0, 0, 0, 0);
    m_setFrameLyt->setSpacing(0);

    m_searchMethodFrame = new QFrame(m_setFrame);
    m_searchMethodFrame->setMinimumWidth(550);
    m_searchMethodLyt = new QHBoxLayout(m_searchMethodFrame);
    m_searchMethodLyt->setContentsMargins(16, 18, 16, 21);
    m_searchMethodFrame->setLayout(m_searchMethodLyt);

    m_descFrame = new QFrame(m_searchMethodFrame);
    m_descFrameLyt = new QVBoxLayout(m_descFrame);
    m_descFrameLyt->setContentsMargins(0, 0, 0, 0);
    m_descFrame->setLayout(m_descFrameLyt);
    m_descLabel1 = new QLabel(m_descFrame);
    m_descLabel2 = new QLabel(m_descFrame);

    //~ contents_path /Search/Create index
    m_descLabel1->setText(tr("Create index"));
    m_descLabel2->setText(tr("Creating index can help you getting results quickly."));
    m_descLabel2->setEnabled(false);
    m_descFrameLyt->addWidget(m_descLabel1);
    m_descFrameLyt->addWidget(m_descLabel2);
    m_searchMethodBtn = new kdk::KSwitchButton(m_searchMethodFrame);
    m_searchMethodLyt->addWidget(m_descFrame);
    m_searchMethodLyt->addStretch();
    m_searchMethodLyt->addWidget(m_searchMethodBtn);
    m_setFrameLyt->addWidget(m_searchMethodFrame);

    QFrame *line = new QFrame(m_setFrame);
    line->setFixedHeight(1);
    line->setLineWidth(0);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    m_setFrameLyt->addWidget(line);

    //设置网页搜索引擎部分的ui
     //~ contents_path /Search/Default web searching engine
    m_webEngineFrame = new ComboxFrame(tr("Default web searching engine"), m_searchMethodFrame);
    m_webEngineFrame->setContentsMargins(8, 0, 8, 0);// ComboxFrame右侧自带8的边距，左右边距各是16所以分别设为8
    m_webEngineFrame->setFixedHeight(56);
    m_webEngineFrame->setMinimumWidth(550);
    m_webEngineFrame->mCombox->insertItem(0, QIcon("/usr/share/ukui-search/search-ukcc-plugin/image/baidu.svg"), tr("baidu"), "baidu");
    m_webEngineFrame->mCombox->insertItem(1, QIcon("/usr/share/ukui-search/search-ukcc-plugin/image/sougou.svg"), tr("sougou"), "sougou");
    m_webEngineFrame->mCombox->insertItem(2, QIcon("/usr/share/ukui-search/search-ukcc-plugin/image/360.svg"), tr("360"), "360");
    m_setFrameLyt->addWidget(m_webEngineFrame);
    m_mainLyt->addWidget(m_setFrame);

    //设置黑名单文件夹部分的ui
    m_blockDirTitleLabel = new TitleLabel(m_pluginWidget);

    //~ contents_path /Search/Block Folders
    m_blockDirTitleLabel->setText(tr("Block Folders"));
    m_blockDirDescLabel = new QLabel(m_pluginWidget);
    m_blockDirDescLabel->setContentsMargins(16, 0, 0, 0);    //TitleLabel自带16边距,QLabel需要自己设
    m_blockDirDescLabel->setEnabled(false);
    m_blockDirDescLabel->setWordWrap(true);
    m_blockDirDescLabel->setText(tr("Following folders will not be searched. You can set it by adding and removing folders."));

    m_blockDirsFrame = new QFrame(m_pluginWidget);
    m_blockDirsFrame->setFrameShape(QFrame::Shape::Box);
    m_blockDirsLyt = new QVBoxLayout(m_blockDirsFrame);
    m_blockDirsLyt->setDirection(QBoxLayout::BottomToTop);
    m_blockDirsFrame->setLayout(m_blockDirsLyt);
    m_blockDirsLyt->setContentsMargins(0, 0, 0, 0);
    m_blockDirsLyt->setSpacing(2);

    QFrame * m_addBlockDirFrame = new QFrame(m_blockDirsFrame);
    m_addBlockDirFrame->setFrameShape(QFrame::Shape::NoFrame);
    m_addBlockDirFrame->setFixedHeight(60);

    m_addBlockDirWidget = new QPushButton(m_addBlockDirFrame);
    m_addBlockDirWidget->setFixedHeight(60);

//        m_addBlockDirWidget->setObjectName("addBlockDirWidget");
//        QPalette pal;
//        QBrush brush = pal.highlight();  //获取window的色值
//        QColor highLightColor = brush.color();
//        QString stringColor = QString("rgba(%1,%2,%3)") //叠加20%白色
//               .arg(highLightColor.red()*0.8 + 255*0.2)
//               .arg(highLightColor.green()*0.8 + 255*0.2)
//               .arg(highLightColor.blue()*0.8 + 255*0.2);

//        m_addBlockDirWidget->setStyleSheet(QString("HoverWidget#addBlockDirWidget{background: palette(button);\
//                                       border-radius: 4px;}\
//                                       HoverWidget:hover:!pressed#addBlockDirWidget{background: %1;  \
//                                       border-radius: 4px;}").arg(stringColor));

    m_addBlockDirWidget->setProperty("useButtonPalette", true);
//    m_addBlockDirWidget->setStyleSheet("QPushButton:!checked{background: palette(base);}");
    m_addBlockDirWidget->setFlat(true);

    m_addBlockDirIcon = new QLabel(m_addBlockDirWidget);
    m_addBlockDirIcon->setPixmap(QIcon("/usr/share/ukui-search/search-ukcc-plugin/image/add.svg").pixmap(12, 12));
    m_addBlockDirIcon->setProperty("useIconHighlightEffect", true);
    m_addBlockDirIcon->setProperty("iconHighlightEffectMode", 1);

    m_addBlockDirLabel = new QLabel(m_addBlockDirWidget);
    m_addBlockDirLabel->setText(tr("Choose folder"));

    m_addBlockDirLyt = new QHBoxLayout(m_addBlockDirWidget);
    m_addBlockDirWidget->setLayout(m_addBlockDirLyt);

    m_blockDirsLyt->addWidget(m_addBlockDirWidget);

    m_addBlockDirLyt->addStretch();
    m_addBlockDirLyt->addWidget(m_addBlockDirIcon);
    m_addBlockDirLyt->addWidget(m_addBlockDirLabel);
    m_addBlockDirLyt->addStretch();
    m_mainLyt->addSpacing(32);
    m_mainLyt->addWidget(m_blockDirTitleLabel);
    m_mainLyt->addWidget(m_blockDirDescLabel);
    m_mainLyt->addWidget(m_blockDirsFrame);
    m_mainLyt->addStretch();
    m_mainLyt->setContentsMargins(0, 0, 0, 0);
}

/**
 * @brief Search::getBlockDirs 从配置文件获取黑名单并将黑名单列表传入
 */
void Search::getBlockDirs()
{
    m_blockDirs.clear();
    if (m_dirSettings)
        m_blockDirs = m_dirSettings->allKeys();
}

/**
 * @brief Search::setBlockDir 尝试写入新的黑名单文件夹
 * @param dirPath 待添加到黑名单的文件夹路径
 * @param is_add 是否是在添加黑名单
 * @return 0成功 !0添加失败的错误代码
 */
int Search::setBlockDir(const QString &dirPath, const bool &is_add)
{
    if (!is_add) {
        if (dirPath.isEmpty()) {
            return ReturnCode::PathEmpty;
        }
        //删除黑名单目录
        m_dirSettings->remove(dirPath);
        removeBlockDirFromList(dirPath);
        return ReturnCode::Succeed;
    }
    if (!dirPath.startsWith(QDir::homePath())) {
        return ReturnCode::NotInHomeDir;
    }

    QString pathKey = dirPath.right(dirPath.length() - 1);

    for (QString dir : m_blockDirs) {
        if (pathKey == dir) {
            return ReturnCode::HasBeenBlocked;
        }

        if (pathKey.startsWith(dir)) {
            return ReturnCode::ParentExist;
        }

        //有它的子文件夹已被添加，删除这些子文件夹
        if (dir.startsWith(pathKey)) {
            m_dirSettings->remove(dir);
            removeBlockDirFromList("/" + dir);
        }
    }
    m_dirSettings->setValue(pathKey, "0");
    appendBlockDirToList(dirPath);
    return ReturnCode::Succeed;
}

/**
 * @brief Search::initBlockDirsList 初始化黑名单列表
 */
void Search::initBlockDirsList()
{
    getBlockDirs();
    for (QString path: m_blockDirs) {
        QString wholePath = QString("/%1").arg(path);
        if (QFileInfo(wholePath).isDir() && path.startsWith("home")) {
            appendBlockDirToList(wholePath);
        }
    }
}

void Search::appendBlockDirToList(const QString &path)
{
    HoverWidget * dirWidget = new HoverWidget(path, m_blockDirsFrame);
    dirWidget->setObjectName(path);
    dirWidget->setMinimumWidth(550);
    dirWidget->setAttribute(Qt::WA_DeleteOnClose);
    QHBoxLayout * dirWidgetLyt = new QHBoxLayout(dirWidget);
    dirWidgetLyt->setSpacing(8);
    dirWidgetLyt->setContentsMargins(0, 0, 0, 0);
    dirWidget->setLayout(dirWidgetLyt);
    QFrame * dirFrame = new QFrame(dirWidget);
    dirFrame->setFrameShape(QFrame::Shape::Box);
    dirFrame->setFixedHeight(50);
    QHBoxLayout * dirFrameLayout = new QHBoxLayout(dirFrame);
    dirFrameLayout->setSpacing(16);
    dirFrameLayout->setContentsMargins(16, 0, 16, 0);
    QLabel * iconLabel = new QLabel(dirFrame);
    QLabel * pathLabel = new QLabel(dirFrame);

    dirFrameLayout->addWidget(iconLabel);
    iconLabel->setPixmap(QIcon::fromTheme("inode-directory").pixmap(QSize(24, 24)));
    pathLabel->setText(path);
    dirFrameLayout->addWidget(pathLabel);
    dirFrameLayout->addStretch();
    QPushButton * delBtn = new QPushButton(dirFrame);
    delBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    delBtn->setFixedSize(30, 30);
    delBtn->setToolTip(tr("delete"));
    delBtn->setProperty("isWindowButton", 0x2);
    delBtn->setProperty("useIconHighlightEffect", 0x8);
    delBtn->setFlat(true);


    delBtn->hide();
    dirFrameLayout->addWidget(delBtn);
    dirWidgetLyt->addWidget(dirFrame);
//    dirWidgetLyt->addWidget(delBtn);

    QFrame *line = new QFrame(m_blockDirsFrame);
    line->setFixedHeight(1);
    line->setLineWidth(0);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
//    m_setFrameLyt->addWidget(line);

    m_blockDirsLyt->addWidget(line);
    m_blockDirsLyt->addWidget(dirWidget);
    connect(delBtn, &QPushButton::clicked, this, [ = ]() {
        setBlockDir(path, false);
        getBlockDirs();
        m_blockDirsLyt->removeWidget(line);
        line->deleteLater();
    });
    connect(dirWidget, &HoverWidget::enterWidget, this, [ = ]() {
        delBtn->show();
    });
    connect(dirWidget, &HoverWidget::leaveWidget, this, [ = ]() {
        delBtn->hide();
    });
}

void Search::removeBlockDirFromList(const QString &path)
{
    HoverWidget * delDirWidget = m_blockDirsFrame->findChild<HoverWidget *>(path);
    if (delDirWidget) {
        qDebug() << "Delete folder succeed! path = " << path;
        delDirWidget->close();
    }
}

void Search::setupConnection()
{
    connect(m_addBlockDirWidget, &QPushButton::clicked, this, &Search::onBtnAddFolderClicked);
}

void Search::onBtnAddFolderClicked()
{
    QFileDialog * fileDialog = new QFileDialog(m_pluginWidget);
//    fileDialog->setFileMode(QFileDialog::Directory); //允许查看文件和文件夹，但只允许选择文件夹
    fileDialog->setFileMode(QFileDialog::DirectoryOnly); //只允许查看文件夹
//    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setDirectory(QDir::homePath());
    fileDialog->setNameFilter(tr("Directories"));
    fileDialog->setWindowTitle(tr("select blocked folder"));
    fileDialog->setLabelText(QFileDialog::Accept, tr("Select"));
    fileDialog->setLabelText(QFileDialog::LookIn, tr("Position: "));
    fileDialog->setLabelText(QFileDialog::FileName, tr("FileName: "));
    fileDialog->setLabelText(QFileDialog::FileType, tr("FileType: "));
    fileDialog->setLabelText(QFileDialog::Reject, tr("Cancel"));
    if(fileDialog->exec() != QDialog::Accepted) {
        fileDialog->deleteLater();
        return;
    }
    QString selectedDir = 0;
    selectedDir = fileDialog->selectedFiles().first();
    qDebug() << "Selected a folder in onBtnAddClicked(): " << selectedDir;
    int returnCode = setBlockDir(selectedDir, true);
    switch (returnCode) {
    case ReturnCode::Succeed :
        qDebug() << "Add blocked folder succeed! path = " << selectedDir;
        getBlockDirs();
        break;
    case ReturnCode::PathEmpty :
        qWarning() << "Add blocked folder failed, choosen path is empty! path = " << selectedDir;
        QMessageBox::warning(m_pluginWidget, tr("Warning"), tr("Add blocked folder failed, choosen path is empty!"));
        break;
    case ReturnCode::NotInHomeDir :
        qWarning() << "Add blocked folder failed, it is not in home path! path = " << selectedDir;
        QMessageBox::warning(m_pluginWidget, tr("Warning"), tr("Add blocked folder failed, it is not in home path!"));
        break;
    case ReturnCode::ParentExist :
        qWarning() << "Add blocked folder failed, its parent dir is exist! path = " << selectedDir;
        QMessageBox::warning(m_pluginWidget, tr("Warning"), tr("Add blocked folder failed, its parent dir is exist!"));
        break;
    case ReturnCode::HasBeenBlocked :
        qWarning() << "Add blocked folder failed, it has been already blocked! path = " << selectedDir;
        QMessageBox::warning(m_pluginWidget, tr("Warning"), tr("Add blocked folder failed, it has been already blocked!"));
        break;
    default:
        break;
    }
}
