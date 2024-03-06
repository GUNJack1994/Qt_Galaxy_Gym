#ifndef USEROBJECT_H
#define USEROBJECT_H

#include <QDate>
#include <QPixmap>
#include "payment.h"
#include "carnettype.h"

class UserObject {

public:
    explicit UserObject();
    UserObject(const QString &name, const QString &surname, const QPixmap &pixmap, const QList<QString > &rfidList, int id, int type = 0);
    QList<Payment> paymentList;
    bool isValid();
    void clear();
    QString name;
    QString surname;
    QString telefon;
    QStringList rfidList;
//    QStringList rfidList;
    QPixmap photo;
    qlonglong id;
    int type;
    QString typeDescription;
    CarnetType carnet;
    QDate validTo;
    QList<CarnetType> family;
    QDateTime entrance;
    int promotion;
};

#endif // USEROBJECT_H
