#ifndef FINDUSERDELEGATE_H
#define FINDUSERDELEGATE_H
#include <QStyledItemDelegate>

class FindUserDelegate : public QStyledItemDelegate
{
public:
    FindUserDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // FINDUSERDELEGATE_H
