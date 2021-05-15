#ifndef INDEXSTATUSRECORDER_H
#define INDEXSTATUSRECORDER_H

#include <QObject>
#include <QSettings>
#include <QDir>
#define CONTENT_INDEX_DATABASE_STATE "content_index_database_state"
#define INDEX_DATABASE_STATE "index_database_state"
#define INOTIFY_NORMAL_EXIT "inotify_normal_exit"
#define INDEX_STATUS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-index-status.conf"
namespace Zeeker {
//fixme: we need a better way to record index status.
class IndexStatusRecorder : public QObject
{
    Q_OBJECT
public:
    static IndexStatusRecorder *getInstance();
    void setStatus(const QString& key, const QVariant& value);
    const QVariant getStatus(const QString& key);

private:
    explicit IndexStatusRecorder(QObject *parent = nullptr);
    QSettings *m_status;
};
}

#endif // INDEXSTATUSRECORDER_H
