/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <xapian.h>
#include <QStandardPaths>
#include <QVector>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QMutex>
#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/index_data").toStdString()
#define CONTENT_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/content_index_data").toStdString()


class FileSearcher : public QObject
{
    Q_OBJECT
public:
    explicit FileSearcher(QObject *parent = nullptr);
    ~FileSearcher();

    static int getCurrentIndexCount();

    static size_t uniqueSymbol1;
    static size_t uniqueSymbol2;
    static size_t uniqueSymbol3;
    static QMutex m_mutex1;
    static QMutex m_mutex2;
    static QMutex m_mutex3;

public Q_SLOTS:
    void onKeywordSearch(QString keyword,QQueue<QString> *searchResultFile,QQueue<QString> *searchResultDir,QQueue<QPair<QString,QStringList>> *searchResultContent);

Q_SIGNALS:
    void resultFile(QQueue<QString> *);
    void resultDir(QQueue<QString> *);
    void resultContent(QQueue<QPair<QString,QStringList>> *);
private:
    int keywordSearchfile(size_t uniqueSymbol, QString keyword, QString value,unsigned slot = 1,int begin = 0, int num = 20);
    int keywordSearchContent(size_t uniqueSymbol, QString keyword, int begin = 0, int num = 20);

    /**
     * @brief FileSearcher::creatQueryForFileSearch
     * This part shall be optimized frequently to provide a more stable search function.
     * @param keyword
     * @param db
     * @return Xapian::Query
     */
    Xapian::Query creatQueryForFileSearch(QString keyword, Xapian::Database &db);
    Xapian::Query creatQueryForContentSearch(QString keyword, Xapian::Database &db);

    int getResult(size_t uniqueSymbol, Xapian::MSet &result, QString value);
    int getContentResult(size_t uniqueSymbol, Xapian::MSet &result,std::string &keyWord);

    bool isBlocked(QString &path);

    QQueue<QString> *m_search_result_file = nullptr;
    QQueue<QString> *m_search_result_dir = nullptr;
    QQueue<QPair<QString,QStringList>> *m_search_result_content = nullptr;
    bool m_searching = false;
};

#endif // FILESEARCHER_H
