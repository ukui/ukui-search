#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include "libsearch_global.h"

class LIBSEARCH_EXPORT GlobalSettings : public QObject
{
    Q_OBJECT
public:
    explicit GlobalSettings(QObject *parent = nullptr);

Q_SIGNALS:

};

#endif // GLOBALSETTINGS_H
