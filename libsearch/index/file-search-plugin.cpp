#include "file-search-plugin.h"
#include "search-manager.h"
using namespace Zeeker;

FileSearchPlugin::FileSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
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
    SearchManager::m_mutex1.lock();
    ++SearchManager::uniqueSymbol1;
    SearchManager::m_mutex1.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        DirectSearch *directSearch;
        directSearch = new DirectSearch(keyword, searchResult, FILE_SEARCH_VALUE, SearchManager::uniqueSymbol1);
        m_pool.start(directSearch);
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileSearch *filesearch;
        filesearch = new FileSearch(searchResult, SearchManager::uniqueSymbol1, keyword, FILE_SEARCH_VALUE, 1, 0, 5);
        m_pool.start(filesearch);
    }
}

QList<SearchPluginIface::Actioninfo> FileSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void FileSearchPlugin::openAction(int actionkey, QString key)
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

DirSearchPlugin::DirSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
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
    SearchManager::m_mutex2.lock();
    ++SearchManager::uniqueSymbol2;
    SearchManager::m_mutex2.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        DirectSearch *directSearch;
        directSearch = new DirectSearch(keyword, searchResult, DIR_SEARCH_VALUE, SearchManager::uniqueSymbol2);
        m_pool.start(directSearch);
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileSearch *filesearch;
        filesearch = new FileSearch(searchResult, SearchManager::uniqueSymbol2, keyword, DIR_SEARCH_VALUE, 1, 0, 5);
        m_pool.start(filesearch);
    }
}

QList<SearchPluginIface::Actioninfo> DirSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void DirSearchPlugin::openAction(int actionkey, QString key)
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

FileContengSearchPlugin::FileContengSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo Openpath { 1, tr("Open path")};
    SearchPluginIface::Actioninfo CopyPath { 2, tr("Copy Path")};
    m_actionInfo << open << Openpath << CopyPath;
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
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
    SearchManager::m_mutex3.lock();
    ++SearchManager::uniqueSymbol3;
    SearchManager::m_mutex3.unlock();

    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        return;
    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        FileContentSearch *fileContentSearch;
        fileContentSearch = new FileContentSearch(searchResult, SearchManager::uniqueSymbol3, keyword, 0, 5);
        m_pool.start(fileContentSearch);
    }
}

QList<SearchPluginIface::Actioninfo> FileContengSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void FileContengSearchPlugin::openAction(int actionkey, QString key)
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
