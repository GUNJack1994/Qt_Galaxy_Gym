#ifndef BASEMANAGER_H
#define BASEMANAGER_H

#include <QObject>
#include <QSettings>
#include <QSqlDatabase>
#include "userobject.h"
#include <QPointer>
#include "carnettype.h"

#define ONETIMEUSER_NORMAL 266
#define ONETIMEUSER_FITNESS_MAN 930
#define ONETIMEUSER_FITNESS_WOMAN 931
#define ONETIMEUSER_OPEN_MAN 932
#define ONETIMEUSER_OPEN_WOMAN 933


class BaseManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged)

public:
    explicit BaseManager(QObject *parent = 0);
    bool isConnected();
    UserObject userByRFID(const QString &rfid);
    UserObject userByID(quint64 id);
    QHash<int, QPixmap>  photoHash();
    QPixmap pixmapFromUserID(int id);
    bool addUser(const UserObject &userObj, qlonglong &newId);
    bool rmUser(quint64 userID);
    bool updateUser(UserObject userObj);
    int freeUserId();
    QSqlDatabase db() const;
    QHash<int, CarnetType> carnetHash;
    UserObject worker;
    static QByteArray pixmapToArray(const QPixmap &pixmap);
    static QPixmap pixmapFromArray(const QByteArray &array);
    void emitNUU();

    bool ready() const;

private:
    QSqlDatabase m_db;

    bool m_ready;

signals:
    void needEntranceUpdate();
    void needUserUpdate();
    void needPaymentUpdate();
    void workerChanged(UserObject arg);

    void readyChanged(bool ready);

public slots:
    void showErrorBox(const QString &text);
    void reconnect();
    bool addEntrance(quint64 userid, QDateTime dateTime, bool valid, bool banan = false);
    bool addWorktime(quint64 userid, QDateTime dateTime);
    bool rmPayment(quint64 paymentID, quint64 userID, QDate &validTo);
    bool addPayment(Payment &payment, QDate &validTo);
    bool isRFIDInUse(const QString &rfid);
    bool isCardIDInUse(int cardID);
    bool updatePaymentForUser(quint64 userID, QDate &validTo);
    QList<Payment> lastPayments(quint64 userId);
    QDateTime lastEntrance(quint64 userID);
    Payment lastPayment(quint64 userId);
    void updateBasicData();
    QList<UserObject> groupedEntrance(QDate fromDate, QDate toDate);
    QList<UserObject> allUserList();
    void setReady(bool ready);
};

#endif // BASEMANAGER_H
