#include "paymentdelegate.h"
#include <QPainter>
#include <QDateTime>
#include "defines.h"

PaymentDelegate::PaymentDelegate()
{
}

void PaymentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if (index.row() == 0 && index.column() == 2) {
            painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor("#3ca0d0"));
    } else if (index.model()->index(index.row(), 2).isValid()
            && index.model()->index(index.row(), 2).data().toDateTime() >= QDateTime::currentDateTime()) {
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_VALID));
    } else {
//        painter->save();
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_INVALID));
//        painter->restore();
    }
    painter->restore();
    QStyledItemDelegate::paint(painter, option, index);
}
