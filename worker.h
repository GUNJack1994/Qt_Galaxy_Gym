#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDate>

class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString surname READ surname WRITE setSurname NOTIFY surnameChanged)
    Q_PROPERTY(qlonglong id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString rfid READ rfid WRITE setRfid NOTIFY rfidChanged)
    Q_PROPERTY(QDate birthdate READ birthdate WRITE setBirthdate NOTIFY birthdateChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit Worker(QObject *parent = 0);
    QString name() const;
    QString surname() const;
    qlonglong id() const;
    QString rfid() const;
    QDate birthdate() const;
    int type() const;

private:
    QString m_name;
    QString m_surname;
    qlonglong m_id;
    QString m_rfid;
    QDate m_birthdate;
    int m_type;

Q_SIGNALS:
    void nameChanged(QString arg);
    void surnameChanged(QString arg);
    void idChanged(qlonglong arg);
    void rfidChanged(QString arg);
    void birthdateChanged(QDate arg);
    void typeChanged(int arg);

public slots:
    void setName(QString arg);
    void setSurname(QString arg);
    void setId(qlonglong arg);
    void setRfid(QString arg);
    void setBirthdate(QDate arg);
    void setType(int arg);
};

#endif // WORKER_H
