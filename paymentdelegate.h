#ifndef PAYMENTDELEGATE_H
#define PAYMENTDELEGATE_H

#include <QStyledItemDelegate>

class PaymentDelegate : public QStyledItemDelegate
{
public:
    PaymentDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PAYMENTDELEGATE_H
