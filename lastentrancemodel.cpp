#include "lastentrancemodel.h"
#include <QDateTime>
#include <QDebug>

LastEntranceModel::LastEntranceModel()
{
}

QVariant LastEntranceModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 4 && role == Qt::DisplayRole)
        return QSqlQueryModel::data(index, role).toDateTime().toString("hh:mm:ss");

    return QSqlQueryModel::data(index, role);
}
