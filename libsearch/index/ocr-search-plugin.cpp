#include "ocr-search-plugin.h"
#include "search-manager.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
using namespace Zeeker;

OcrSearchPlugin::OcrSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString OcrSearchPlugin::name()
{
    return "Ocr Search";
}

const QString OcrSearchPlugin::description()
{
    return tr("Ocr Search");
}

QString OcrSearchPlugin::getPluginName()
{
    return tr("Ocr Search");
}

void Zeeker::OcrSearchPlugin::KeywordSearch(QString keyword, DataQueue<ResultInfo> *searchResult)
{
    SearchManager::m_mutexOcr.lock();
    ++SearchManager::uniqueSymbolOcr;
    SearchManager::m_mutexOcr.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        return;
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        OcrSearch *ocrsearch;
        ocrsearch = new OcrSearch(searchResult, SearchManager::uniqueSymbolOcr, keyword,  0, 5);
        m_pool.start(ocrsearch);
    }
}

void OcrSearchPlugin::stopSearch()
{
    SearchManager::m_mutexOcr.lock();
    ++SearchManager::uniqueSymbolOcr;
    SearchManager::m_mutexOcr.unlock();
}

QList<SearchPluginIface::Actioninfo> OcrSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void OcrSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    qDebug() << "openAction!!!!!!!!";
    switch (actionkey) {
    case 0:
        if(FileUtils::openFile(key) == -1) {
            QMessageBox msgBox(m_detailPage);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Can not get a default application for opening %1.").arg(key));
            msgBox.exec();
        }
        break;
    case 1:
        FileUtils::openFile(key, true);
        break;
    case 2:
        FileUtils::copyPath(key);
    default:
        break;
    }
}

QWidget *OcrSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
//    m_iconLabel->setPixmap(ri.icon.pixmap(120, 120));
    QPixmap pixmap;
    int with = m_iconLabel->width();
    int height = m_iconLabel->height();
    pixmap.load(m_currentActionKey);

//    pixmap.scaledToHeight(10);
    pixmap.scaled(with, height, Qt::KeepAspectRatio/*, Qt::SmoothTransformation*/);
    m_iconLabel->setPixmap(pixmap);
    m_iconLabel->setScaledContents(true);
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(FileUtils::setAllTextBold(showname));
    //if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    //}
    m_pluginLabel->setText(tr("Ocr"));

    m_pathLabel2->setText(m_pathLabel2->fontMetrics().elidedText(m_currentActionKey, Qt::ElideRight, m_pathLabel2->width()));
    m_pathLabel2->setToolTip(m_currentActionKey);
    m_timeLabel2->setText(ri.description.at(2).value);
//    m_timeLabel2->setText(m_timeLabel2->fontMetrics().elidedText(ri.description.at(2).value, Qt::ElideRight, m_timeLabel2->width()));
    return m_detailPage;
}

void OcrSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(360);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(128);
    m_iconLabel->setFixedWidth(280);
    m_iconLabel->setContentsMargins(40, 0 ,0 ,10);

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

    m_line_1 = new SeparationLine(m_detailPage);

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

    m_line_2 = new SeparationLine(m_detailPage);

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
        if(FileUtils::openFile(m_currentActionKey) == -1) {
            QMessageBox msgBox(m_detailPage);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Can not get a default application for opening %1.").arg(m_currentActionKey));
            msgBox.exec();
        }
    });
    connect(m_actionLabel2, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::openFile(m_currentActionKey, true);
    });
    connect(m_actionLabel3, &ActionLabel::actionTriggered, [ & ](){
        FileUtils::copyPath(m_currentActionKey);
    });
}
