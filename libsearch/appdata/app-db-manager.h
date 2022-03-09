#ifndef APPDBMANAGER_H
#define APPDBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
namespace UkuiSearch {
/**
 * @brief The AppDBManager class
 * 功能：1.遍历并且监听desktop文件目录，建立并且维护应用信息数据库。
 *      2.监听应用安装，打开事件，收藏等事件，更新数据库
 */

class AppDBManager : public QObject
{
    Q_OBJECT
public:
    static AppDBManager *getInstance();
    void initDateBaseConnection();
private:
    explicit AppDBManager(QObject *parent = nullptr);
    ~AppDBManager();
    QSqlDatabase m_database;

};
}

#endif // APPDBMANAGER_H
