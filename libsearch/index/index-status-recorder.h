#ifndef INDEXSTATUSRECORDER_H
#define INDEXSTATUSRECORDER_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QMutex>
#define CONTENT_INDEX_DATABASE_STATE "content_index_database_state"
#define INDEX_DATABASE_STATE "index_database_state"
#define OCR_DATABASE_STATE "ocr_database_state"
#define INOTIFY_NORMAL_EXIT "inotify_normal_exit" // 1 - 出错；2 - 正常；3-关闭索引； 0-有信号正在处理
#define PENDING_FILE_QUEUE_FINISH "pending_file_queue_finish"
#define INDEX_STATUS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-index-status.conf"
namespace UkuiSearch {
//fixme: we need a better way to record index status.
class IndexStatusRecorder : public QObject
{
    Q_OBJECT
public:
    static IndexStatusRecorder *getInstance();
    void setStatus(const QString& key, const QVariant& value);
    const QVariant getStatus(const QString& key);
    bool indexDatabaseEnable();
    bool contentIndexDatabaseEnable();
    bool ocrDatabaseEnable();

private:
    explicit IndexStatusRecorder(QObject *parent = nullptr);
    static IndexStatusRecorder *m_instance;
    QSettings *m_status;
    QMutex m_mutex;
};
}

#endif // INDEXSTATUSRECORDER_H
