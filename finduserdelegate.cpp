#include "finduserdelegate.h"
#include <QPainter>
#include <QDate>
#include "defines.h"

FindUserDelegate::FindUserDelegate()
{

}

void FindUserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if ( (index.model()->index(index.row(), 3).isValid()) &&
         index.model()->index(index.row(), 3).data().toDate() < QDate::currentDate()) {
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_INVALID));
    } else {
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_VALID));
    }
    painter->restore();
    QStyledItemDelegate::paint(painter, option, index);
}

