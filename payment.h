#ifndef PAYMENT_H
#define PAYMENT_H

#include <QObject>
#include <QDate>

class Payment
{
public:
    explicit Payment();
    qlonglong id;
    qlonglong user;
    QDate to;
    QDateTime date;
    qreal price;
    int type;
    qreal discount;
    qlonglong worker;
    QString ticketTypeDescription;
    bool isValid();
    static Payment currentPayment(QList<Payment> paymentList);
    int promotion;

};

#endif // PAYMENT_H
