#include "basemanager.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QBuffer>
#include <QPixmap>
#include <QUrl>
#include <QMessageBox>
#include "application.h"
#include "defines.h"
#include "math.h"
#define READER 0
#define BANANA_READER 1

BaseManager::BaseManager(QObject *parent) :
    QObject(parent),
    m_ready(false)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    reconnect();
}

bool BaseManager::isConnected()
{
    if (db().isValid() && (db().isOpen() || db().open()))
        return true;

    qWarning() << "Nie można połączyć się z bazą";
    return false;
}

UserObject BaseManager::userByRFID(const QString &rfid)
{
    if (!isConnected())
        return UserObject();

    UserObject userObj;
    QSqlQuery userQuerry(db());

    if (!userQuerry.exec(QString("SELECT users.id, users.name, users.surname, users.type, users.carnettype, users.validto, users.promotion, photos.photo, users.family"
                                 " FROM users"
                                 " INNER JOIN rfids ON rfids.user=users.id"
                                 " INNER JOIN photos ON photos.user=users.id"
                                 " WHERE rfids.rfid = '%1'").arg(rfid))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkownika" << userQuerry.lastError().text();
        return userObj;
    }
    //todo zabezpieczyc przed wyciekiem
    if (userQuerry.next()) {
        userObj.id = userQuerry.value(0).toLongLong();
        userObj.name = userQuerry.value(1).toString();
        userObj.surname = userQuerry.value(2).toString();
        userObj.type = userQuerry.value(3).toInt();
        if (carnetHash.contains(userQuerry.value(4).toInt()))
            userObj.carnet = carnetHash.value(userQuerry.value(4).toInt());

        userObj.validTo = userQuerry.value(5).toDate();
        userObj.promotion = userQuerry.value(6).toInt();
        userObj.photo = pixmapFromArray(userQuerry.value(7).toByteArray());

        Q_FOREACH (QString type, userQuerry.value(8).toString().split(";")) {
            int pos = type.toInt();
            if (carnetHash.contains(type.toInt()))
                userObj.family << carnetHash.value(type.toInt());
        }

    }

    // rfid

    if (!userQuerry.exec(QString("SELECT rfids.rfid"
                                 " FROM rfids"
                                 " INNER JOIN users ON rfids.user=users.id"
                                 " WHERE rfids.rfid = '%1'").arg(rfid))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkownika" << userQuerry.lastError().text();
        return userObj;
    }

    userQuerry.first();
    while (userQuerry.isValid()) {
        userObj.rfidList << userQuerry.value(0).toString();
        userQuerry.next();
    }

    return userObj;
}

UserObject BaseManager::userByID(quint64 id)
{
    if (!isConnected())
        return UserObject();

    UserObject userObj;
    QSqlQuery userQuerry(db());

    if (!userQuerry.exec(QString("SELECT users.id, users.name, users.surname, users.type, users.carnettype, users.validto, users.promotion, photos.photo, users.family"
                                 " FROM users"
                                 " INNER JOIN rfids ON rfids.user=users.id"
                                 " INNER JOIN photos ON photos.user=users.id"
                                 " WHERE users.id = '%1'").arg(id))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkownika" << userQuerry.lastError().text();
        return userObj;
    }


    if (userQuerry.next()) {
        userObj.id = userQuerry.value(0).toLongLong();
        userObj.name = userQuerry.value(1).toString();
        userObj.surname = userQuerry.value(2).toString();
        userObj.type = userQuerry.value(3).toInt();

        if (carnetHash.contains(userQuerry.value(4).toInt()))
            userObj.carnet = carnetHash.value(userQuerry.value(4).toInt());


        userObj.validTo = userQuerry.value(5).toDate();
        userObj.promotion = userQuerry.value(6).toInt();
        userObj.photo = pixmapFromArray(userQuerry.value(7).toByteArray());

        if (!userQuerry.value(7).toString().isEmpty()) {
            Q_FOREACH (QString type, userQuerry.value(8).toString().split(";")) {
                if (carnetHash.contains(type.toInt()))
                    userObj.family << carnetHash.value(type.toInt());
            }

        }
    }

    if (!userQuerry.exec(QString("SELECT rfids.rfid"
                                 " FROM rfids"
                                 " INNER JOIN users ON rfids.user=users.id"
                                 " WHERE users.id = '%1'").arg(id))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkownika" << userQuerry.lastError().text();
        return userObj;
    }

    userQuerry.first();
    while (userQuerry.isValid()) {
        userObj.rfidList <<userQuerry.value(0).toString();
        userQuerry.next();
    }

    return userObj;
}

QHash<int, QPixmap> BaseManager::photoHash()
{
    if (!isConnected())
        return QHash<int, QPixmap>();

    QSqlQuery userQuerry(m_db);
    QHash<int, QPixmap> pixList;

    if (!userQuerry.exec("SELECT users.id, photos.photo FROM photos INNER JOIN users on users.id=photos.user WHERE users.type=0"))
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkownika" << userQuerry.lastError().text();


    userQuerry.first();
    while (userQuerry.isValid()) {
        pixList.insert(userQuerry.value(0).toInt(), pixmapFromArray(userQuerry.value(1).toByteArray()).scaled(100,100));
        userQuerry.next();
    }

    return pixList;
}

QPixmap BaseManager::pixmapFromUserID(int id)
{
    if (!isConnected())
        return QPixmap();

    QSqlQuery userQuerry(db());

    if (!userQuerry.exec(QString("SELECT photo FROM photos WHERE user='%1'").arg(id))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o zdjecie" << id << userQuerry.lastError().text();
        return QPixmap();
    }

    if (userQuerry.next())
        return pixmapFromArray(userQuerry.value(0).toByteArray());

    return QPixmap();
}

bool BaseManager::addUser(const UserObject &userObj, qlonglong &newId)
{
    if (!isConnected())
        return false;

    if (userObj.rfidList.isEmpty()) {
        qWarning() << "nie mozna dodac uzytkownika bez rfid";
        return false;
    }
    db().transaction();
    QSqlQuery userAddQuerry(db());


    if (newId != -1) {
        userAddQuerry.prepare("INSERT INTO users (id, name, surname, type, carnettype, family)"
                              "VALUES (:id, :name, :surname, :type, :carnettype, :family)");
    } else {
        userAddQuerry.prepare("INSERT INTO users (name, surname, type, carnettype, family)"
                              "VALUES (:name, :surname, :type, :carnettype, :family)");
    }
    // user
    userAddQuerry.bindValue(":id", newId);
    userAddQuerry.bindValue(":name", userObj.name);
    userAddQuerry.bindValue(":surname", userObj.surname);
    userAddQuerry.bindValue(":type", userObj.type);
    userAddQuerry.bindValue(":carnettype", userObj.carnet.id);

    QStringList family;
    Q_FOREACH (CarnetType carnet, userObj.family) {
        family << QString::number(carnet.id);
    }

    userAddQuerry.bindValue(":family", family.join(";"));

    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania dodającego użytkownika:" << userAddQuerry.lastError().text();
        return false;
    }

    userAddQuerry.finish();

    if (newId == -1) {
        // user id
        userAddQuerry.prepare("SELECT LAST_INSERT_ID()");
        if (!userAddQuerry.exec()) {
            db().rollback();
            qWarning() << "LAST id error";
            return false;
        }

        newId = userAddQuerry.lastInsertId().value<quint64>();
        userAddQuerry.finish();
    }

    // rfid
    for (int a = 0; a < userObj.rfidList.count(); ++a) {
        QString rfid = userObj.rfidList.at(a);
        userAddQuerry.prepare("INSERT INTO rfids (rfid, user)"
                              "VALUES (:rfid, :user)");
        userAddQuerry.bindValue(":rfid", rfid);
        userAddQuerry.bindValue(":user", newId);
        if (!userAddQuerry.exec()) {
            qWarning() << "Błąd zapytania dodającego rfid:" << userAddQuerry.lastError().text();
            db().rollback();
            return false;
        }
    }
    userAddQuerry.finish();


    // photo
    userAddQuerry.prepare("INSERT INTO photos (user, photo)"
                          "VALUES (:user, :photo)");
    userAddQuerry.bindValue(":user", newId);
    userAddQuerry.bindValue(":photo", pixmapToArray(userObj.photo));

    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania dodającego photo:" << userAddQuerry.lastError().text();
        db().rollback();
        return false;
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    userAddQuerry.finish();

    return code;
}

bool BaseManager::rmUser(quint64 userID)
{
    if (!isConnected()) {
        qWarning() << "brak połączenia z bazą" << db().lastError().text();
        return false;
    }
    db().transaction();
    QSqlQuery rmQuerry(db());

    // user
    rmQuerry.prepare("DELETE FROM users WHERE id=:id");
    rmQuerry.bindValue(":id", userID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego użytkownika:" << rmQuerry.lastError().text();
        return false;
    }

    // photo
    rmQuerry.prepare("DELETE FROM photos WHERE user=:id");
    rmQuerry.bindValue(":id", userID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego zdjecie:" << rmQuerry.lastError().text();
        return false;
    }

    //rfid
    rmQuerry.prepare("DELETE FROM rfids WHERE user=:id");
    rmQuerry.bindValue(":id", userID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego rfid:" << rmQuerry.lastError().text();
        return false;
    }

    //payment
    rmQuerry.prepare("DELETE FROM payment WHERE user=:id");
    rmQuerry.bindValue(":id", userID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego payment:" << rmQuerry.lastError().text();
        return false;
    }

    //entrance
    rmQuerry.prepare("DELETE FROM entrance WHERE user=:id");
    rmQuerry.bindValue(":id", userID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego entrance:" << rmQuerry.lastError().text();
        return false;
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    return code;
}

bool BaseManager::updateUser(UserObject userObj)
{
    if (!isConnected() || !userObj.isValid() || !userObj.carnet.isValid())
        return false;

    db().transaction();
    QSqlQuery userAddQuerry(db());

    // user
    userAddQuerry.prepare("UPDATE users SET name=:name, surname=:surname, type=:type, carnettype=:carnettype, validto=:validto, promotion=:promotion, family=:family WHERE id=:id");
    userAddQuerry.bindValue(":id", userObj.id);
    userAddQuerry.bindValue(":name", userObj.name);
    userAddQuerry.bindValue(":surname", userObj.surname);
    userAddQuerry.bindValue(":type", userObj.type);
    userAddQuerry.bindValue(":carnettype", userObj.carnet.id);
    userAddQuerry.bindValue(":validto", userObj.validTo.toString("yyyy-MM-dd"));
    userAddQuerry.bindValue(":promotion", userObj.promotion);

    QStringList family;
    Q_FOREACH (CarnetType carnet, userObj.family)
        family << QString::number(carnet.id);

    userAddQuerry.bindValue(":family", family.join(";"));

    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania edytujacego użytkownika:" << userAddQuerry.lastError().text();
        db().rollback();
        return false;
    }

    // photo
    userAddQuerry.prepare("UPDATE photos SET photo=:photo WHERE user=:id");
    userAddQuerry.bindValue(":id", userObj.id);
    userAddQuerry.bindValue(":photo", pixmapToArray(userObj.photo));

    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania edytujacego photo:" << userAddQuerry.lastError().text();
        db().rollback();
        return false;
    }


    // rfid clear
    userAddQuerry.prepare("DELETE FROM rfids WHERE user=:user");
    userAddQuerry.bindValue(":user", userObj.id);
    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego rfid z tabeli" << userAddQuerry.lastError().text();
        db().rollback();
        return false;
    }

    // rfid
    for (int a = 0; a < userObj.rfidList.count(); ++a) {
        QString rfid = userObj.rfidList.at(a);
        userAddQuerry.prepare("INSERT INTO rfids (rfid, user) VALUES (:rfid, :user)");
        userAddQuerry.bindValue(":rfid", rfid);
        userAddQuerry.bindValue(":user", userObj.id);
        if (!userAddQuerry.exec()) {
            qWarning() << "Błąd zapytania dodającego rfid:" << userAddQuerry.lastError().text();
            db().rollback();
            return false;
        }
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    return code;
}

int BaseManager::freeUserId()
{
    if (!isConnected())
        return -1;

    int value = -1;

    QSqlQuery userAddQuerry(db());

    // user
    userAddQuerry.prepare("SELECT t1.id+1 AS Missing "
                          "FROM users AS t1 "
                          "LEFT JOIN users AS t2 ON t1.id+1 = t2.id "
                          "WHERE t2.id IS NULL "
                          "ORDER BY t1.id LIMIT 1");

    if (!userAddQuerry.exec()) {
        qWarning() << "Błąd zapytania sprawdzajacego wolne id:" << userAddQuerry.lastError().text();
        return false;
    }


    if (userAddQuerry.next())
        value = userAddQuerry.value(0).toInt();

    return value;
}

QSqlDatabase BaseManager::db() const
{
    return m_db;
}

QByteArray BaseManager::pixmapToArray(const QPixmap &pixmap)
{
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "JPG");
    return buffer.buffer();
}

QPixmap BaseManager::pixmapFromArray(const QByteArray &array)
{
    QPixmap pixmap;
    pixmap.loadFromData(array, "JPG");
    return pixmap;
}

void BaseManager::emitNUU()
{
    //todo co to jest? !!!!
    Q_EMIT needUserUpdate();
}

bool BaseManager::ready() const
{
    return m_ready;
}

void BaseManager::showErrorBox(const QString &text)
{
    QMessageBox *mBox = new QMessageBox();
    mBox->setAttribute(Qt::WA_DeleteOnClose);
    mBox->setWindowTitle(tr("Błąd"));
    mBox->setText(text);
    mBox->setIcon(QMessageBox::Critical);
    mBox->exec();
}

void BaseManager::reconnect()
{
    db().setHostName(app->settings->value("DataBase/address", QString(" ")).toString());
    db().setDatabaseName(app->settings->value("DataBase/base", QString("")).toString());
    db().setUserName(app->settings->value("DataBase/login", QString("")).toString());
    db().setPassword(app->settings->value("DataBase/pass", QString("")).toString());

    setReady(db().isValid() && db().open());
    if (ready())
        updateBasicData();

}

bool BaseManager::addEntrance(quint64 id, QDateTime dateTime, bool valid, bool banan)
{
    if (!isConnected())
        return false;

    db().transaction();
    QSqlQuery entranceAddQuerry(db());

    entranceAddQuerry.prepare("INSERT INTO entrance (user, date, valid, reader)"
                              "VALUES (?, ?, ?, ?)");
    entranceAddQuerry.addBindValue(id);
    entranceAddQuerry.addBindValue(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    entranceAddQuerry.addBindValue(valid);
    entranceAddQuerry.addBindValue(banan ? BANANA_READER : READER);
    if (!entranceAddQuerry.exec()) {
        qWarning() << "entrance BLAD SQL:" << entranceAddQuerry.lastError().text();
        return false;
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    Q_EMIT needEntranceUpdate();

    return code;
}

bool BaseManager::addWorktime(quint64 userid, QDateTime dateTime)
{
    if (!isConnected())
        return false;

    db().transaction();
    QSqlQuery entranceAddQuerry(db());

    entranceAddQuerry.prepare("INSERT INTO worktime (user, date)"
                              "VALUES (?, ?)");
    entranceAddQuerry.addBindValue(userid);
    entranceAddQuerry.addBindValue(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    if (!entranceAddQuerry.exec()) {
        qWarning() << "worktime BLAD SQL:" << entranceAddQuerry.lastError().text();
        return false;
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    Q_EMIT needEntranceUpdate();
    return code;
}

bool BaseManager::rmPayment(quint64 paymentID, quint64 userID, QDate &validTo)
{
    if (!isConnected())
        return false;

    db().transaction();

    // user
    QSqlQuery rmQuerry(db());
    rmQuerry.prepare("DELETE FROM payment WHERE id=:id");
    rmQuerry.bindValue(":id", paymentID);

    if (!rmQuerry.exec()) {
        qWarning() << "Błąd zapytania usuwajacego płatnosc:" << rmQuerry.lastError().text();
        return false;
    }

    if (userID != ONETIMEUSER_NORMAL
            || userID != ONETIMEUSER_FITNESS_MAN
            || userID != ONETIMEUSER_FITNESS_WOMAN
            || userID != ONETIMEUSER_OPEN_MAN
            || userID != ONETIMEUSER_OPEN_WOMAN) {
        // jednorazowy
        updatePaymentForUser(userID, validTo);
    }

    bool code = db().commit();
    if (!code)
        db().rollback();

    Q_EMIT needPaymentUpdate();
    return code;
}

bool BaseManager::addPayment(Payment &payment, QDate &validTo)
{
    if (!isConnected())
        return false;

    db().transaction();
    QSqlQuery paymentAddQuerry(db());

    paymentAddQuerry.prepare("INSERT INTO payment (user, date, dateto, type, price, discount, worker)"
                             " VALUES (:user, :date, :to, :type, :price, :discount, :worker)");
    paymentAddQuerry.bindValue(":user", payment.user);
    paymentAddQuerry.bindValue(":date", payment.date);
    paymentAddQuerry.bindValue(":to", payment.to);
    paymentAddQuerry.bindValue(":type", payment.type);
    paymentAddQuerry.bindValue(":price", qMax(payment.price - payment.promotion, (qreal)0));
    paymentAddQuerry.bindValue(":discount", payment.discount);
    paymentAddQuerry.bindValue(":worker", payment.worker);

    if (!paymentAddQuerry.exec()) {
        qWarning() << "BLAD SQL:" << paymentAddQuerry.lastError().text();
        db().rollback();
        return false;
    }


    if (payment.user != ONETIMEUSER_NORMAL
            || payment.user != ONETIMEUSER_FITNESS_MAN
            || payment.user != ONETIMEUSER_FITNESS_WOMAN
            || payment.user != ONETIMEUSER_OPEN_MAN
            || payment.user != ONETIMEUSER_OPEN_WOMAN) {
        // oprocz jednorazowego
        updatePaymentForUser(payment.user, validTo);
    }

    bool code = db().commit();
    if (!code)
        db().rollback();


    Q_EMIT needPaymentUpdate();
    return code;
}

bool BaseManager::isRFIDInUse(const QString &rfid)
{
    if (!isConnected())
        return false;

    QSqlQuery checkQuerry(db());
    checkQuerry.prepare("SELECT * FROM rfids WHERE (rfid=:rfid)");
    checkQuerry.bindValue(":rfid", rfid);
    if (!checkQuerry.exec()) {
        qWarning() << "BLAD odpytania o uzycie rfid:" << checkQuerry.lastError().text();
        return false;
    }

    return checkQuerry.next();
}

bool BaseManager::isCardIDInUse(int cardID)
{
    if (!isConnected())
        return false;

    QSqlQuery checkQuerry(db());
    checkQuerry.prepare("SELECT * FROM rfids WHERE (cardid=:cardid)");
    checkQuerry.bindValue(":cardid", cardID);
    if (!checkQuerry.exec()) {
        qWarning() << "BLAD odpytania o uzycie cardid" << checkQuerry.lastError().text();
        return false;
    }

    return checkQuerry.next();
}

bool BaseManager::updatePaymentForUser(quint64 userID, QDate &validTo)
{
    Payment paymentLast = lastPayment(userID);

    UserObject user = userByID(paymentLast.user);
    user.validTo = paymentLast.to;
    validTo = paymentLast.to;
    return updateUser(user);

}

QList<Payment> BaseManager::lastPayments(quint64 userId)
{
    if (!isConnected())
        return QList<Payment>();

    QSqlQuery paymentCheckQuerry(db());
    QList<Payment> paymentList;

    paymentCheckQuerry.prepare("SELECT * FROM payment INNER JOIN types ON payment.type=types.id WHERE (user=:userid) ORDER BY payment.dateto DESC LIMIT 5");
    paymentCheckQuerry.bindValue(":userid", userId);

    if (!paymentCheckQuerry.exec()) {
        qWarning() << "BLAD odpytania o platnosc SQL:" << paymentCheckQuerry.lastError().text();
    }

    paymentCheckQuerry.first();
    while (paymentCheckQuerry.isValid()) {
        Payment payment;
        payment.id = paymentCheckQuerry.value(0).toLongLong();
        payment.user = paymentCheckQuerry.value(1).toLongLong();
        payment.date = paymentCheckQuerry.value(2).toDateTime();
        payment.to = paymentCheckQuerry.value(3).toDate();
        payment.type = paymentCheckQuerry.value(4).toInt();
        payment.price = paymentCheckQuerry.value(5).toDouble();
        payment.discount = paymentCheckQuerry.value(6).toDouble();
        payment.worker = paymentCheckQuerry.value(7).toLongLong();
        payment.ticketTypeDescription = paymentCheckQuerry.value(10).toString();
        paymentList.append(payment);
        paymentCheckQuerry.next();
    }

    return paymentList;
}

QDateTime BaseManager::lastEntrance(quint64 userID)
{
    if (!isConnected())
        return QDateTime();

    QSqlQuery paymentCheckQuerry(db());

    paymentCheckQuerry.prepare("SELECT date FROM entrance WHERE (user=:userid) ORDER BY date DESC LIMIT 1");
    paymentCheckQuerry.bindValue(":userid", userID);
    if (!paymentCheckQuerry.exec()) {
        qWarning() << "BLAD odpytania o ostatnie wejscie:" << paymentCheckQuerry.lastError().text();
    }

    if (paymentCheckQuerry.next()) {
        return paymentCheckQuerry.value(0).toDateTime();
    }
    return QDateTime();
}

Payment BaseManager::lastPayment(quint64 userId)
{
    if (!isConnected())
        return Payment();

    QSqlQuery paymentCheckQuerry(db());
    Payment payment;

    paymentCheckQuerry.prepare("SELECT * FROM payment INNER JOIN types ON payment.type=types.id WHERE (user=:userid) ORDER BY payment.dateto DESC LIMIT 1");
    paymentCheckQuerry.bindValue(":userid", userId);
    paymentCheckQuerry.bindValue(":currentdate", QDate::currentDate());
    if (!paymentCheckQuerry.exec()) {
        qWarning() << "BLAD odpytania o platnosc SQL:" << paymentCheckQuerry.lastError().text();
    }

    payment.user = userId;
    paymentCheckQuerry.first();
    while (paymentCheckQuerry.isValid()) {
        payment.id = paymentCheckQuerry.value(0).toLongLong();
        payment.date = paymentCheckQuerry.value(2).toDateTime();
        payment.to = paymentCheckQuerry.value(3).toDate();
        payment.type = paymentCheckQuerry.value(4).toInt();
        payment.price = paymentCheckQuerry.value(5).toDouble();
        payment.discount = paymentCheckQuerry.value(6).toDouble();
        payment.worker = paymentCheckQuerry.value(7).toLongLong();
        payment.ticketTypeDescription = paymentCheckQuerry.value(10).toString();
        paymentCheckQuerry.next();
    }

    return payment;
}

void BaseManager::updateBasicData()
{
    if (!isConnected())
        return;

    QSqlQuery typesQuerry(db());

    if (!typesQuerry.exec(QString("SELECT * FROM types"))) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o typy" << typesQuerry.lastError().text();
        return;
    }

    carnetHash.clear();

    typesQuerry.first();
    while (typesQuerry.isValid()) {
        CarnetType carnet;
        carnet.id = typesQuerry.value(0).value<quint64>();
        carnet.options = typesQuerry.value(1).toString();
        carnet.description = typesQuerry.value(2).toString();
        Q_FOREACH (QString option, carnet.options.split(";")) {
            QStringList vals = option.split(":");
            if (vals.count() == 2) {
                carnet.prices.insert(vals.first(), vals.last().toDouble());
            }
        }
        carnet.multi = typesQuerry.value(3).toInt();
        carnet.colorString = typesQuerry.value(4).toString();
        carnet.hidden = typesQuerry.value(5).toBool();
        carnet.enabled = typesQuerry.value(6).toBool();
        carnetHash.insert(carnet.id, carnet);
        typesQuerry.next();
    }
}

QList<UserObject>  BaseManager::groupedEntrance(QDate fromDate, QDate toDate)
{

    QSqlQuery geQuerry(db());

    geQuerry.prepare(QString("SELECT entrance.user, entrance.valid, users.surname, users.name, entrance.date, entrance.reader, users.carnettype"
                             " FROM entrance"
                             " INNER JOIN users ON entrance.user=users.id"
                             " WHERE entrance.date>='%1'"
                             " AND entrance.reader<='%2'"
                             " AND (users.carnettype=%3 OR users.carnettype=%4)"
                             " ORDER BY entrance.date ASC")
                     .arg(fromDate.toString("yyyy-MM-dd 00:00:00"))
                     .arg(toDate.toString("yyyy-MM-dd 23:59:59"))
                     .arg(CARNET_FITENSS_WOMAN)
                     .arg(CARNET_OPEN_WOMAN));

    if (!geQuerry.exec()) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o wejscia grupowe" << geQuerry.lastError().text();
        QList<UserObject> ();
    }

    qDebug() << geQuerry.lastQuery();
    geQuerry.first();

    QList<UserObject> userList;
    while (geQuerry.isValid()) {

        UserObject user;
        user.name = geQuerry.value(2).toString();
        user.surname = geQuerry.value(3).toString();
        user.id = geQuerry.value(0).toInt();
        user.entrance = geQuerry.value(4).toDateTime();

        geQuerry.next();
        userList << user;
    }

    return userList;
}

QList<UserObject> BaseManager::allUserList()
{
    QSqlQuery geQuerry(db());

    geQuerry.prepare(QString("SELECT id, surname, name FROM users ORDER BY surname ASC"));

    if (!geQuerry.exec()) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o użytkowników" << geQuerry.lastError().text();
        QList<UserObject> ();
    }

    geQuerry.first();

    QList<UserObject> userList;
    while (geQuerry.isValid()) {

        UserObject user;
        user.name = geQuerry.value(2).toString();
        user.surname = geQuerry.value(1).toString();
        user.id = geQuerry.value(0).toInt();

        geQuerry.next();
        userList << user;
    }

    return userList;
}

void BaseManager::setReady(bool ready)
{
    if (m_ready == ready)
        return;

    m_ready = ready;
    emit readyChanged(ready);
}
