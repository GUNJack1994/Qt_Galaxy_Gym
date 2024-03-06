#include "querythread.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "basemanager.h"
#include "application.h"


QueryThread::QueryThread()
{
    m_db = QSqlDatabase::addDatabase("QMYSQL", "second");
    m_db.setHostName(app->settings->value("DataBase/address", QString(" ")).toString());
    m_db.setDatabaseName(app->settings->value("DataBase/base", QString("")).toString());
    m_db.setUserName(app->settings->value("DataBase/login", QString("")).toString());
    m_db.setPassword(app->settings->value("DataBase/pass", QString("")).toString());

    checkConnection();

    setLock(false);
}

bool QueryThread::isReady()
{
    return !m_lock;
}

void QueryThread::abort()
{
    m_abort = true;
}

bool QueryThread::lock() const
{
    return m_lock;
}

void QueryThread::getPhotoList(const QString &params)
{
    if (!checkConnection())
        return;

    m_abort = false;
    setLock(true);

    QSqlQuery userQuerry(m_db);
    QHash<int, QPixmap> pixList;
    QString stringQuery = QString("SELECT users.id, photos.photo FROM photos INNER JOIN users on users.id=photos.user WHERE users.type=0 %1 ORDER BY photos.user ASC").arg(params);

    if (!userQuerry.exec(stringQuery)) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o liste zdjec" << userQuerry.lastError().text();
        setLock(false);
        return;
    }

    userQuerry.first();
    while (userQuerry.isValid() && !m_abort) {
        pixList.insert(userQuerry.value(0).toInt(), BaseManager::pixmapFromArray(userQuerry.value(1).toByteArray()).scaled(126,126));
        userQuerry.next();
    }

    setLock(false);

    if (!m_abort)
        emit photoListReady(pixList);
}

void QueryThread::getUserList(const QString &params)
{
    if (!checkConnection())
        return;

    m_abort = false;

    setLock(true);

    QSqlQuery userQuerry(m_db);
    QList< VHash > pixList;
    QString stringQuery = params;

    if (!userQuerry.exec(stringQuery)) {
        qWarning() << Q_FUNC_INFO << "Błędne zapytanie o liste uzytkownikow" << userQuerry.lastError().text() << m_db.connectionName();
        setLock(false);
        return;
    }

    userQuerry.first();
    while (userQuerry.isValid() && !m_abort) {
        QHash<int, QVariant> hash;

        hash.insert(0, userQuerry.value(0).toInt());
        hash.insert(1, userQuerry.value(1).toString());
        hash.insert(2, userQuerry.value(2).toString());
        hash.insert(3, userQuerry.value(3).toDateTime());

        pixList.append(hash);
        userQuerry.next();
    }

    setLock(false);

    if (!m_abort)
        emit userListReady(pixList);

}

void QueryThread::setLock(bool lock)
{
    if (m_lock == lock)
        return;

    m_lock = lock;
    emit lockChanged(lock);
}

bool QueryThread::checkConnection()
{
    if (m_db.isValid() && (m_db.isOpen() || m_db.open()))
        return true;

    qWarning() << "Nie można połączyć się z bazą";
    return false;
}

