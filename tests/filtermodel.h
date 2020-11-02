#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <KRunner/RunnerManager>
#include <QTimer>
#include "declarative/runnermodel.h"

namespace Plasma
{
    class RunnerManager;
    class QueryMatch;
} // namespace Plasma
class appfiltermodel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    appfiltermodel();
    QVariant headerData(int section,Qt::Orientation orientation ,int role)const override;
    RunnerModel *runnerModel;
protected:
    bool filterAcceptsRow(int source_row,const QModelIndex & source_parent)const override;

private:
    int appct;
    void add_appct();
Q_SIGNALS:
    int appcount();

};

class filefiltermodel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    filefiltermodel();
    bool filterAcceptsRow(int source_row,const QModelIndex & source_parent)const override;
    QVariant headerData(int section,Qt::Orientation orientation ,int role) const override;
    RunnerModel *runnerModel1;

    bool state();
    bool x;





};

#endif // FILTERMODEL_H
