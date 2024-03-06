#include "entrancedelegate.h"
#include <QPainter>
#include "defines.h"

EntranceDelegate::EntranceDelegate()
{
}

void EntranceDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if ( (index.model()->index(index.row(), 1).isValid()) &&
         !index.model()->index(index.row(), 1).data().toBool()) {
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_INVALID));
    } else {
        painter->fillRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), QColor(COLOR_VALID));
    }
    painter->restore();
    QStyledItemDelegate::paint(painter, option, index);
}
