#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QTreeView>
#include <QHeaderView>
#include <QGSettings>

class fileview : public QTreeView
{
public:
    fileview();
    ~fileview();
private:
    //控制style的gsetting
    QGSettings *style_settings;
    //用来判断style
    QStringList stylelist;

    void styleChange();
};

#endif // FILEVIEW_H
