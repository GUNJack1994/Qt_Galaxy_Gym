#include "payment.h"
#include <QDebug>

Payment::Payment() :
    id(-1),
    ticketTypeDescription("--/--"),
    price(-1),
    promotion(0)
{
}

bool Payment::isValid()
{
    return id >= 0;
}

Payment Payment::currentPayment(QList<Payment> paymentList)
{
    Q_FOREACH (Payment payment, paymentList)
        if (payment.to >= QDate::currentDate())
            return payment;

    return Payment();
}
