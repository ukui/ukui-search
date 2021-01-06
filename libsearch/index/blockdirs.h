#ifndef BLOCKDIRS_H
#define BLOCKDIRS_H

#include <QObject>
#include "global-settings.h"

#ifndef MYTYPE
#define MYTYPE
#define MYDIR 0
#define MYFILE 1
#endif // MYTYPE

#ifndef SETVALUERESULT
#define SETVALUERESULT
#define DONOTHING 1
#define FAILED -1
#define SUCCESSED 0
#endif // SETVALUERESULT

class BlockDirs : public QObject
{
    Q_OBJECT
public:
    explicit BlockDirs(QObject *parent = nullptr);
    int setValue(const QString&, const int&);
    int removeValue(const QString&, const int&);
    int getValues();
Q_SIGNALS:

};

#endif // BLOCKDIRS_H
