#include "index-status-recorder.h"
#include <mutex>

using namespace UkuiSearch;
IndexStatusRecorder *IndexStatusRecorder::m_instance = nullptr;
std::once_flag g_IndexStatusRecorderInstanceFlag;
IndexStatusRecorder *IndexStatusRecorder::getInstance()
{
    std::call_once(g_IndexStatusRecorderInstanceFlag, [] () {
        m_instance = new IndexStatusRecorder;
    });
    return m_instance;
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

bool IndexStatusRecorder::indexDatabaseEnable()
{
    m_mutex.lock();
    m_status->sync();
    m_mutex.unlock();
    return m_status->value(INDEX_DATABASE_STATE, QVariant(false)).toBool();

}

bool IndexStatusRecorder::contentIndexDatabaseEnable()
{
    m_mutex.lock();
    m_status->sync();
    m_mutex.unlock();
    return m_status->value(CONTENT_INDEX_DATABASE_STATE, QVariant(false)).toBool();

}

bool IndexStatusRecorder::ocrDatabaseEnable()
{
    m_mutex.lock();
    m_status->sync();
    m_mutex.unlock();
    return m_status->value(OCR_DATABASE_STATE, QVariant(false)).toBool();

}

IndexStatusRecorder::IndexStatusRecorder(QObject *parent) : QObject(parent)
{
    m_status = new QSettings(INDEX_STATUS, QSettings::IniFormat, this);
}
