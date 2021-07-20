#ifndef DATAQUEUE_H
#define DATAQUEUE_H
#include <QMutex>
#include <QMutexLocker>
#include <QList>
#include "libsearch_global.h"
namespace Zeeker {
// TODO I want a unlocked queue
template <typename T>
class LIBSEARCH_EXPORT DataQueue : public QList<T>
{
public:
    inline void enqueue(const T &t) {
        QMutexLocker locker(&m_mutex);
        QList<T>::append(t);
    }
    inline T dequeue() {
        QMutexLocker locker(&m_mutex);
        return QList<T>::takeFirst();
    }
private:
    QMutex m_mutex;
};
}

#endif // DATAQUEUE_H
