#ifndef ENTRANCEDELEGATE_H
#define ENTRANCEDELEGATE_H

#include <QStyledItemDelegate>

class EntranceDelegate : public QStyledItemDelegate
{
public:
    EntranceDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ENTRANCEDELEGATE_H
