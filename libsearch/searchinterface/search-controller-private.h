#ifndef SEARCHCONTROLLERPRIVATE_H
#define SEARCHCONTROLLERPRIVATE_H

#include <QMutex>
#include <QMap>
#include "search-controller.h"
namespace UkuiSearch {

class SearchControllerPrivate
{
public:
    explicit SearchControllerPrivate(SearchController *parent);
    ~SearchControllerPrivate();
    size_t refreshSearchId();
    DataQueue<ResultItem>* refreshDataqueue();
    DataQueue<ResultItem>* initDataQueue();

    void addSearchDir(QString &path);
    void setRecurse(bool recurse = true);
    void addKeyword(QString &keyword);
    void setActiveKeywordSegmentation(bool active);
    void addFileLabel(QString &label);
    void setOnlySearchFile(bool onlySearchFile);
    void setOnlySearchDir(bool onlySearchDir);
    void setSearchOnlineApps(bool searchOnlineApps);

    size_t getCurrentSearchId();
    DataQueue<ResultItem>* getDataQueue();
    ResultDataTypes getResultDataType(SearchType searchType);
    QStringList getCustomResultDataType(QString customSearchType);
    bool beginSearchIdCheck(size_t searchId);
    void finishSearchIdCheck();
    void stop();
    QStringList getSearchDir();
    bool isRecurse();
    QStringList getKeyword();
    bool isKeywordSegmentationActived();
    QStringList getFileLabel();
    bool isSearchFileOnly();
    bool isSearchDirOnly();
    bool isSearchOnlineApps();
    void clearAllConditions();
    void clearKeyWords();
    void clearSearchDir();
    void clearFileLabel();

    bool setResultDataType(SearchType searchType, ResultDataTypes dataType);
    bool setCustomResultDataType(QString customSearchType, QStringList dataType);

    /**
     * @brief 分页选项
     * @param first 指定起始位置
     * @param maxResults 每次搜索结果集的数量
     */
    void setPagination(unsigned int first, unsigned int maxResults);
    unsigned int first() const;
    unsigned int maxResults() const;

private:
    void copyData();
    //TODO: 这里是否可以改为字节对齐的写法？
//    DataQueue<ResultItem>* m_dataQueue = nullptr ;
    std::shared_ptr<DataQueue<ResultItem>> m_sharedDataQueue = nullptr;
    size_t m_searchId = 0;
    QMutex m_searchIdMutex;
    SearchController *q = nullptr;
    std::shared_ptr<SearchController> m_formerController = nullptr;

    QStringList m_keywords;
    QStringList m_searchDirs;
    QStringList m_FileLabels;
    bool m_recurse = true;
    bool m_activeKeywordSegmentation = false;
    bool m_onlySearchFile = false;
    bool m_onlySearchDir = false;
    bool m_searchOnlineApps = false;

    unsigned int m_first = 0;
    unsigned int m_maxResults = 100;        //默认取100条结果

    QMap<SearchType, ResultDataTypes> m_searchType2ResultDataType;
    QMap<QString, QStringList> m_customSearchType2ResultDataType;
};
}

#endif // SEARCHCONTROLLERPRIVATE_H
