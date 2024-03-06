#ifndef ROWFILTER_H
#define ROWFILTER_H

#include <QSortFilterProxyModel>

class RowFilter : public QSortFilterProxyModel
{
public:
    RowFilter();
    QList<int> ignoreColumnList;

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

#endif // ROWFILTER_H
