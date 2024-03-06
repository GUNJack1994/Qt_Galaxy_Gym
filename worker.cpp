#include "worker.h"

Worker::Worker(QObject *parent) :
    QObject(parent)
{
}

QString Worker::name() const
{
    return m_name;
}

QString Worker::surname() const
{
    return m_surname;
}

qlonglong Worker::id() const
{
    return m_id;
}

QString Worker::rfid() const
{
    return m_rfid;
}

QDate Worker::birthdate() const
{
    return m_birthdate;
}

int Worker::type() const
{
    return m_type;
}

void Worker::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void Worker::setSurname(QString arg)
{
    if (m_surname != arg) {
        m_surname = arg;
        emit surnameChanged(arg);
    }
}

void Worker::setId(qlonglong arg)
{
    if (m_id != arg) {
        m_id = arg;
        emit idChanged(arg);
    }
}

void Worker::setRfid(QString arg)
{
    if (m_rfid != arg) {
        m_rfid = arg;
        emit rfidChanged(arg);
    }
}

void Worker::setBirthdate(QDate arg)
{
    if (m_birthdate != arg) {
        m_birthdate = arg;
        emit birthdateChanged(arg);
    }
}

void Worker::setType(int arg)
{
    if (m_type != arg) {
        m_type = arg;
        emit typeChanged(arg);
    }
}
