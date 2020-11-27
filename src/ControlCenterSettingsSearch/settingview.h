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

private:
    //控制style的gsetting
    QGSettings *style_settings;
    //用来判断style
    QStringList stylelist;
    void styleChange();

};

#endif // SETTINGVIEW_H
