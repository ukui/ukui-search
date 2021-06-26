#include "index-status-recorder.h"

using namespace Zeeker;
static IndexStatusRecorder *global_instance_indexStatusRecorder = nullptr;

IndexStatusRecorder *IndexStatusRecorder::getInstance()
{
    if(!global_instance_indexStatusRecorder) {
        global_instance_indexStatusRecorder = new IndexStatusRecorder;
    }
    return global_instance_indexStatusRecorder;
}

void IndexStatusRecorder::setStatus(const QString &key, const QVariant &value)
{
    m_mutex.lock();
    m_status->setValue(key, value);
    m_status->sync();
    m_mutex.unlock();
}

const QVariant IndexStatusRecorder::getStatus(const QString &key)
{
    return m_status->value(key);
}

IndexStatusRecorder::IndexStatusRecorder(QObject *parent) : QObject(parent)
{
    m_status = new QSettings(INDEX_STATUS, QSettings::IniFormat, this);
}
