#ifndef PHOTODELEGATOR_H
#define PHOTODELEGATOR_H
#include <QStyledItemDelegate>

class PhotoDelegator : public QStyledItemDelegate
{
public:
    PhotoDelegator();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // QAbstractItemDelegate interface
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PHOTODELEGATOR_H
