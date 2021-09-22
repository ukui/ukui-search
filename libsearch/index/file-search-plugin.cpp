#include "file-search-plugin.h"
#include "search-manager.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
using namespace Zeeker;

FileSearchPlugin::FileSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString FileSearchPlugin::name()
{
    return tr("File Search");
}

const QString FileSearchPlugin::description()
{
    return tr("File search.");
}

QString FileSearchPlugin::getPluginName()
{
    return tr("File Search");
}

void Zeeker::FileSearchPlugin::KeywordSearch(QString keyword, DataQueue<ResultInfo> *searchResult)
{
    SearchManager::m_mutexFile.lock();
    ++SearchManager::uniqueSymbolFile;
    SearchManager::m_mutexFile.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        DirectSearch *directSearch;
        directSearch = new DirectSearch(keyword, searchResult, FILE_SEARCH_VALUE, SearchManager::uniqueSymbolFile);
        m_pool.start(directSearch);
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileSearch *filesearch;
        filesearch = new FileSearch(searchResult, SearchManager::uniqueSymbolFile, keyword, FILE_SEARCH_VALUE, 1, 0, 5);
        m_pool.start(filesearch);
    }
}

QList<SearchPluginIface::Actioninfo> FileSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void FileSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    switch (actionkey) {
    case 0:
        FileUtils::openFile(key);
        break;
    case 1:
        FileUtils::openFile(key, true);
    case 2:
        FileUtils::copyPath(key);
    default:
        break;
    }
}

QWidget *FileSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(240, 240));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(FileUtils::escapeHtml(showname)));
    if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    }
    m_pluginLabel->setText(tr("File"));

    m_pathLabel2->setText(m_pathLabel2->fontMetrics().elidedText(m_currentActionKey, Qt::ElideRight, m_pathLabel2->width()));
    m_pathLabel2->setToolTip(m_currentActionKey);
    m_timeLabel2->setText(ri.description.at(1).value);
    return m_detailPage;
}

void FileSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(421);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(240);

    m_nameFrame = new QFrame(m_detailPage);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(280);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_pluginLabel->setEnabled(false);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new QFrame(m_detailPage);
    m_line_1->setLineWidth(0);
    m_line_1->setFixedHeight(1);
    m_line_1->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_pathFrame = new QFrame(m_detailPage);
    m_pathFrameLyt = new QHBoxLayout(m_pathFrame);
    m_pathLabel1 = new QLabel(m_pathFrame);
    m_pathLabel2 = new QLabel(m_pathFrame);
    m_pathLabel1->setText(tr("Path"));
    m_pathLabel2->setFixedWidth(240);
    m_pathLabel2->setAlignment(Qt::AlignRight);
    m_pathFrameLyt->addWidget(m_pathLabel1);
    m_pathFrameLyt->addStretch();
    m_pathFrameLyt->addWidget(m_pathLabel2);

    m_timeFrame = new QFrame(m_detailPage);
    m_timeFrameLyt = new QHBoxLayout(m_timeFrame);
    m_timeLabel1 = new QLabel(m_timeFrame);
    m_timeLabel2 = new QLabel(m_timeFrame);
    m_timeLabel2->setAlignment(Qt::AlignRight);
    m_timeLabel1->setText(tr("Last time modified"));
    m_timeFrameLyt->addWidget(m_timeLabel1);
    m_timeFrameLyt->addStretch();
    m_timeFrameLyt->addWidget(m_timeLabel2);

    m_line_2 = new QFrame(m_detailPage);
    m_line_2->setLineWidth(0);
    m_line_2->setFixedHeight(1);
    m_line_2->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_actionLabel1 = new ActionLabel(tr("Open"), m_currentActionKey, m_actionFrame);
    m_actionLabel2 = new ActionLabel(tr("Open path"), m_currentActionKey, m_actionFrame);
    m_actionLabel3 = new ActionLabel(tr("Copy path"), m_currentActionKey, m_actionFrame);

    m_actionFrameLyt->addWidget(m_actionLabel1);
    m_actionFrameLyt->addWidget(m_actionLabel2);
    m_actionFrameLyt->addWidget(m_actionLabel3);
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(50);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addWidget(m_nameFrame);
    m_detailLyt->addWidget(m_line_1);
    m_detailLyt->addWidget(m_pathFrame);
    m_detailLyt->addWidget(m_timeFrame);
    m_detailLyt->addWidget(m_line_2);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey);
    });
    connect(m_actionLabel2, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey, true);
    });
    connect(m_actionLabel3, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::copyPath(m_currentActionKey);
    });
}

//bool FileSearchPlugin::isPreviewEnable(QString key, int type)
//{
//    return true;
//}

//QWidget *FileSearchPlugin::previewPage(QString key, int type, QWidget *parent)
//{
//    QWidget *previewPage = new QWidget(parent);
//    QHBoxLayout * previewLyt = new QHBoxLayout(previewPage);
//    previewLyt->setContentsMargins(0, 0, 0, 0);
//    QLabel *label = new QLabel(previewPage);
//    previewLyt->addWidget(label);
//    label->setFixedHeight(120);
//    previewPage->setFixedSize(120,120);
//    previewLyt->setAlignment(Qt::AlignCenter);
//    label->setPixmap(FileUtils::getFileIcon(QUrl::fromLocalFile(key).toString()).pixmap(120,120));
//    return previewPage;
//}

DirSearchPlugin::DirSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString DirSearchPlugin::name()
{
    return tr("Dir Search");
}

const QString DirSearchPlugin::description()
{
    return tr("Dir search.");
}

QString DirSearchPlugin::getPluginName()
{
    return tr("Dir Search");
}

void Zeeker::DirSearchPlugin::KeywordSearch(QString keyword, DataQueue<ResultInfo> *searchResult)
{
    SearchManager::m_mutexDir.lock();
    ++SearchManager::uniqueSymbolDir;
    SearchManager::m_mutexDir.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        DirectSearch *directSearch;
        directSearch = new DirectSearch(keyword, searchResult, DIR_SEARCH_VALUE, SearchManager::uniqueSymbolDir);
        m_pool.start(directSearch);
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileSearch *filesearch;
        filesearch = new FileSearch(searchResult, SearchManager::uniqueSymbolDir, keyword, DIR_SEARCH_VALUE, 1, 0, 5);
        m_pool.start(filesearch);
    }
}

QList<SearchPluginIface::Actioninfo> DirSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void DirSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    switch (actionkey) {
    case 0:
        FileUtils::openFile(key);
        break;
    case 1:
        FileUtils::openFile(key, true);
    case 2:
        FileUtils::copyPath(key);
    default:
        break;
    }
}

QWidget *DirSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(240, 240));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(FileUtils::escapeHtml(showname)));
    if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    }
    m_pluginLabel->setText(tr("directory"));

    m_pathLabel2->setText(m_pathLabel2->fontMetrics().elidedText(m_currentActionKey, Qt::ElideRight, m_pathLabel2->width()));
    m_pathLabel2->setToolTip(m_currentActionKey);
    m_timeLabel2->setText(ri.description.at(1).value);
    return m_detailPage;
}

void DirSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(421);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(240);

    m_nameFrame = new QFrame(m_detailPage);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(280);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_pluginLabel->setEnabled(false);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new QFrame(m_detailPage);
    m_line_1->setLineWidth(0);
    m_line_1->setFixedHeight(1);
    m_line_1->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_pathFrame = new QFrame(m_detailPage);
    m_pathFrameLyt = new QHBoxLayout(m_pathFrame);
    m_pathLabel1 = new QLabel(m_pathFrame);
    m_pathLabel2 = new QLabel(m_pathFrame);
    m_pathLabel1->setText(tr("Path"));
    m_pathLabel2->setFixedWidth(240);
    m_pathLabel2->setAlignment(Qt::AlignRight);
    m_pathFrameLyt->addWidget(m_pathLabel1);
    m_pathFrameLyt->addStretch();
    m_pathFrameLyt->addWidget(m_pathLabel2);

    m_timeFrame = new QFrame(m_detailPage);
    m_timeFrameLyt = new QHBoxLayout(m_timeFrame);
    m_timeLabel1 = new QLabel(m_timeFrame);
    m_timeLabel2 = new QLabel(m_timeFrame);
    m_timeLabel2->setAlignment(Qt::AlignRight);
    m_timeLabel1->setText(tr("Last time modified"));
    m_timeFrameLyt->addWidget(m_timeLabel1);
    m_timeFrameLyt->addStretch();
    m_timeFrameLyt->addWidget(m_timeLabel2);

    m_line_2 = new QFrame(m_detailPage);
    m_line_2->setLineWidth(0);
    m_line_2->setFixedHeight(1);
    m_line_2->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_actionLabel1 = new ActionLabel(tr("Open"), m_currentActionKey, m_actionFrame);
    m_actionLabel2 = new ActionLabel(tr("Open path"), m_currentActionKey, m_actionFrame);
    m_actionLabel3 = new ActionLabel(tr("Copy path"), m_currentActionKey, m_actionFrame);

    m_actionFrameLyt->addWidget(m_actionLabel1);
    m_actionFrameLyt->addWidget(m_actionLabel2);
    m_actionFrameLyt->addWidget(m_actionLabel3);
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(50);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addWidget(m_nameFrame);
    m_detailLyt->addWidget(m_line_1);
    m_detailLyt->addWidget(m_pathFrame);
    m_detailLyt->addWidget(m_timeFrame);
    m_detailLyt->addWidget(m_line_2);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey);
    });
    connect(m_actionLabel2, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey, true);
    });
    connect(m_actionLabel3, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::copyPath(m_currentActionKey);
    });
}

//bool DirSearchPlugin::isPreviewEnable(QString key, int type)
//{
//    return false;
//}

//QWidget *DirSearchPlugin::previewPage(QString key, int type, QWidget *parent)
//{
//    return nullptr;
//}

FileContengSearchPlugin::FileContengSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString FileContengSearchPlugin::name()
{
    return tr("File Content Search");
}

const QString FileContengSearchPlugin::description()
{
    return tr("File content search.");
}

QString FileContengSearchPlugin::getPluginName()
{
    return tr("File content search");
}

void Zeeker::FileContengSearchPlugin::KeywordSearch(QString keyword, DataQueue<ResultInfo> *searchResult)
{
    SearchManager::m_mutexContent.lock();
    ++SearchManager::uniqueSymbolContent;
    SearchManager::m_mutexContent.unlock();

    m_keyWord = keyword;
    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        return;
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileContentSearch *fileContentSearch;
        fileContentSearch = new FileContentSearch(searchResult, SearchManager::uniqueSymbolContent, keyword, 0, 5);
        m_pool.start(fileContentSearch);
    }
}

QList<SearchPluginIface::Actioninfo> FileContengSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void FileContengSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    switch (actionkey) {
    case 0:
        FileUtils::openFile(key);
        break;
    case 1:
        FileUtils::openFile(key, true);
    case 2:
        FileUtils::copyPath(key);
    default:
        break;
    }
}

QWidget *FileContengSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(120, 120));

    m_pluginLabel->setText(tr("File"));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(FileUtils::escapeHtml(showname)));
    if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    }

    m_snippetLabel->setText(getHtmlText(wrapData(m_snippetLabel,ri.description.at(0).value), m_keyWord));
    m_pathLabel2->setText(m_pathLabel2->fontMetrics().elidedText(m_currentActionKey, Qt::ElideRight, m_pathLabel2->width()));
    m_pathLabel2->setToolTip(m_currentActionKey);
    m_timeLabel2->setText(ri.description.at(2).value);
    return m_detailPage;
}

QString FileContengSearchPlugin::getHtmlText(const QString &text, const QString &keyword)
{
    QString htmlString;
    bool boldOpenned = false;
    for(int i = 0; i < text.length(); i++) {
        if((keyword.toUpper()).contains(QString(text.at(i)).toUpper())) {
            if(! boldOpenned) {
                boldOpenned = true;
                htmlString.append(QString("<b><font size=\"4\">"));
            }
            htmlString.append(FileUtils::escapeHtml(QString(text.at(i))));
        } else {
            if(boldOpenned) {
                boldOpenned = false;
                htmlString.append(QString("</font></b>"));
            }
            htmlString.append(FileUtils::escapeHtml(QString(text.at(i))));
        }
    }
    htmlString.replace("\n", "<br />");//替换换行符
    return htmlString;
}

QString FileContengSearchPlugin::wrapData(QLabel *p_label, const QString &text)
{
    QString wrapText = text;

    QFontMetrics fontMetrics = p_label->fontMetrics();
    int textSize = fontMetrics.width(wrapText);

    if(textSize > LABEL_MAX_WIDTH){
        int lastIndex = 0;
        int count = 0;

        for(int i = lastIndex; i < wrapText.length(); i++) {

            if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) == LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i, '\n');
                count++;
            } else if(fontMetrics.width(wrapText.mid(lastIndex, i - lastIndex)) > LABEL_MAX_WIDTH) {
                lastIndex = i;
                wrapText.insert(i - 1, '\n');
                count++;
            } else {
                continue;
            }

            if(count == 2){
                break;
            }
        }
    }
    p_label->setText(wrapText);
    return wrapText;
}

void FileContengSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(360);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(128);

    m_nameFrame = new QFrame(m_detailPage);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(280);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_pluginLabel->setEnabled(false);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new QFrame(m_detailPage);
    m_line_1->setLineWidth(0);
    m_line_1->setFixedHeight(1);
    m_line_1->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_snippetLabel = new QLabel(m_detailPage);
//    m_snippetLabel->setWordWrap(true);
    m_snippetLabel->setContentsMargins(8, 0, 8, 0);

    m_pathFrame = new QFrame(m_detailPage);
    m_pathFrameLyt = new QHBoxLayout(m_pathFrame);
    m_pathLabel1 = new QLabel(m_pathFrame);
    m_pathLabel2 = new QLabel(m_pathFrame);
    m_pathLabel1->setText(tr("Path"));
    m_pathLabel2->setFixedWidth(240);
    m_pathLabel2->setAlignment(Qt::AlignRight);
    m_pathFrameLyt->addWidget(m_pathLabel1);
    m_pathFrameLyt->addStretch();
    m_pathFrameLyt->addWidget(m_pathLabel2);

    m_timeFrame = new QFrame(m_detailPage);
    m_timeFrameLyt = new QHBoxLayout(m_timeFrame);
    m_timeLabel1 = new QLabel(m_timeFrame);
    m_timeLabel2 = new QLabel(m_timeFrame);
    m_timeLabel2->setAlignment(Qt::AlignRight);
    m_timeLabel1->setText(tr("Last time modified"));
    m_timeFrameLyt->addWidget(m_timeLabel1);
    m_timeFrameLyt->addStretch();
    m_timeFrameLyt->addWidget(m_timeLabel2);

    m_line_2 = new QFrame(m_detailPage);
    m_line_2->setLineWidth(0);
    m_line_2->setFixedHeight(1);
    m_line_2->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_actionLabel1 = new ActionLabel(tr("Open"), m_currentActionKey, m_actionFrame);
    m_actionLabel2 = new ActionLabel(tr("Open path"), m_currentActionKey, m_actionFrame);
    m_actionLabel3 = new ActionLabel(tr("Copy path"), m_currentActionKey, m_actionFrame);

    m_actionFrameLyt->addWidget(m_actionLabel1);
    m_actionFrameLyt->addWidget(m_actionLabel2);
    m_actionFrameLyt->addWidget(m_actionLabel3);
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(50);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addWidget(m_nameFrame);
    m_detailLyt->addWidget(m_line_1);
    m_detailLyt->addWidget(m_snippetLabel);
    m_detailLyt->addWidget(m_pathFrame);
    m_detailLyt->addWidget(m_timeFrame);
    m_detailLyt->addWidget(m_line_2);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey);
    });
    connect(m_actionLabel2, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey, true);
    });
    connect(m_actionLabel3, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::copyPath(m_currentActionKey);
    });
}

//bool FileContengSearchPlugin::isPreviewEnable(QString key, int type)
//{
//    return false;
//}

//QWidget *FileContengSearchPlugin::previewPage(QString key, int type, QWidget *parent)
//{
//    return nullptr;
//}
