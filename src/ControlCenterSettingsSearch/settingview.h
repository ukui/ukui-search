#ifndef SETTINGVIEW_H
#define SETTINGVIEW_H

#include <QTreeView>
#include "../Style/style.h"
#include <QDebug>

class settingview : public QTreeView
{
public:
    settingview();
    ~settingview();

public Q_SLOTS:
    void changesize(int row);

};

#endif // SETTINGVIEW_H
