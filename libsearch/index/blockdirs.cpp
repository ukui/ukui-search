#include "blockdirs.h"

//优先级先放一下
BlockDirs::BlockDirs(QObject *parent) : QObject(parent)
{
    return;
}

int BlockDirs::setValue(const QString& path, const int &type)
{
    return FAILED;
}

int BlockDirs::removeValue(const QString& path, const int &type)
{
    return FAILED;
}

int BlockDirs::getValues()
{
    return FAILED;
}
