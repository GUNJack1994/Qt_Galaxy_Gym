#include "rowfilter.h"
#include <QDebug>
#include <QModelIndex>

RowFilter::RowFilter()
{
}

bool RowFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    if (filterRegExp().isEmpty() || filterRegExp().pattern().isEmpty()) {
        return true;
    }

    bool ret = false;

    for (int col = 0; col < columnCount(); ++col) {
        if (!ignoreColumnList.contains(col) && sourceModel()->index(source_row,col).data().toString().contains(filterRegExp().pattern(), Qt::CaseInsensitive)) {
            ret = true;
            return true;
        }
    }

    return ret;
}
