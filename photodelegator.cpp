#include "photodelegator.h"
#include <QPainter>
#include "basemanager.h"

PhotoDelegator::PhotoDelegator()
{

}

void PhotoDelegator::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    painter->save();
    if ( index.isValid()) {
        QPixmap pixmap = index.data().value<QPixmap>();
        painter->drawPixmap(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), pixmap);
//        painter->drawText(option.rect, QString::number(index.data(Qt::UserRole).toInt()));
    }
//    painter->restore();
}

QSize PhotoDelegator::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    return QSize(200,200);
}

