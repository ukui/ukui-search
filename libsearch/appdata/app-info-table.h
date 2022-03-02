#ifndef APPINFOTABLE_H
#define APPINFOTABLE_H

#include <QObject>
namespace UkuiSearch {
class AppInfoTablePrivate;
/**
 * @brief The AppInfoTable class
 * TODO:提供查询接口（待定），包括：
 * 1.查询全部已安装应用信息（图标，名称，分类等），并且根据系统语言切换
 * 2.查询收藏应用信息
 * 3.查询置顶顺序信息
 * 4.收藏顺序修改
 * 5.置顶顺序修改
 * 6.添加到桌面快捷方式
 * 7.固定到任务栏快捷方式
 * 8.应用启动
 * 9.应用卸载
 * 注意事项：修改接口实现时注意事务操作
 */
class AppInfoTable : public QObject
{
    Q_OBJECT
public:
    explicit AppInfoTable(QObject *parent = nullptr);
    AppInfoTable(AppInfoTable &) = delete;
    AppInfoTable &operator =(const AppInfoTable &) = delete;

private:
    AppInfoTablePrivate *d;

};
}

#endif // APPINFOTABLE_H
