#include "userobject.h"
#include <QDebug>

UserObject::UserObject() :
    name(QString()),
    surname(QString()),
    telefon(QString()),
    id(-1),
    type(-1),
    promotion(0)
{
}

UserObject::UserObject(const QString &name, const QString &surname, const QPixmap &pixmap, const QList<QString > &rfidPairList, int id, int type)
{
    this->name = name;
    this->surname = surname;
    this->telefon = telefon;
    this->photo = pixmap;
    this->rfidList = rfidPairList;
    this->id = id;
    this->type = type;

}

bool UserObject::isValid()
{
    return id >= 0;
}

void UserObject::clear()
{
    id = -1;
}
