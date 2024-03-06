#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QSettings>
#include <QSerialPort>

class RFIDManager : public QObject
{
    Q_OBJECT
public:
//
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged)
    explicit RFIDManager(QObject *parent = 0);
    ~RFIDManager();
    void reconnect();

    bool ready() const;

private:
    QTimer cardTimer;
    QTimer readTimer;
    QSerialPort m_serialPort;
    QString lastID;
    QByteArray m_readedArray;

    bool m_ready;

Q_SIGNALS:
    void card(const QString &id);
//    void status(bool);

    void readyChanged(bool ready);

private slots:
    void onCardTimeout();
    void onReadTimeout();


public slots:
    void onReadReady();


    void setReady(bool ready);
};

Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::StopBits)

#endif // RFIDMANAGER_H
