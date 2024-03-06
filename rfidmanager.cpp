#include "rfidmanager.h"
#include <QDebug>
#include "application.h"

RFIDManager::RFIDManager(QObject *parent) :
    QObject(parent),
    m_ready(false)
{
    connect(&m_serialPort, SIGNAL(readyRead()), SLOT(onReadReady()));

    cardTimer.setInterval(2000);
    cardTimer.setSingleShot(true);
    connect(&cardTimer, SIGNAL(timeout()), SLOT(onCardTimeout()));

    readTimer.setInterval(1000);
    readTimer.setSingleShot(true);
    connect(&readTimer, SIGNAL(timeout()), SLOT(onReadTimeout()));

    reconnect();
}

RFIDManager::~RFIDManager()
{
}

void RFIDManager::reconnect()
{
    // TODO obsluga bledow, statusow
    if (m_serialPort.isOpen())
        m_serialPort.close();

    m_serialPort.setPortName(app->settings->value("SerialPort/name", "").toString());
    m_serialPort.setBaudRate(app->settings->value("SerialPort/baudrate", -1).toInt());
    m_serialPort.setDataBits(static_cast<QSerialPort::DataBits>(app->settings->value("SerialPort/databits", -1).toInt()));
    m_serialPort.setParity(static_cast<QSerialPort::Parity>(app->settings->value("SerialPort/parity", -1).toInt()));
    m_serialPort.setStopBits(static_cast<QSerialPort::StopBits>(app->settings->value("SerialPort/stopbits", -1).toInt()));

    bool isOpen = m_serialPort.open(QSerialPort::ReadOnly);
    if (isOpen) {
        qDebug() << "Otworzono port szeregowy";
    } else {
        qDebug() << "Nie można otworzyć portu" << m_serialPort.portName() << m_serialPort.dataBits() << m_serialPort.parity() << m_serialPort.stopBits() << m_serialPort.errorString();

    }
    setReady(isOpen);
}

bool RFIDManager::ready() const
{
    return m_ready;
}

void RFIDManager::onCardTimeout()
{
    //qDebug() << "Card timeout";
    lastID.clear();
    cardTimer.stop();
}

void RFIDManager::onReadTimeout()
{
    qWarning() << "read timeout";
    m_readedArray.clear();
}

void RFIDManager::onReadReady()
{
    m_readedArray.append(m_serialPort.readAll());
    qDebug() << "readed: " << m_readedArray.toHex();


    if (m_readedArray.size() > 10) {
        qWarning() << "data to long";
        m_readedArray.clear();
        return;
    } else if (m_readedArray.size() < 10) {
        qWarning() << "fragment of data" << m_readedArray.size();
        readTimer.start();
        return;
    }







//    if (m_readedArray.size() > 5) {
//        qWarning() << "data to long" << m_readedArray.toHex();
//        m_readedArray.clear();
//        return;
//    } else if (m_readedArray.size() < 5) {
//        qWarning() << "fragment of data" << m_readedArray.size();
//        readTimer.start();
//        return;
//    }

    readTimer.stop();
//    qDebug() << "readed_array" << QString::fromUtf8(m_readedArray.toHex());
//    QString id = QString::fromUtf8(m_readedArray.toHex());
    QString id = QString::fromUtf8(m_readedArray).trimmed().replace(" ","");
    m_readedArray.clear();

    qDebug() << "R:" << id;

    if (id != lastID) {
        qDebug() << "ID:" << id;
        lastID = id;
        emit card(id);
        //        emit card(QString::fromUtf8(m_readedArray.toHex()));
    }
    cardTimer.start();
}

void RFIDManager::setReady(bool ready)
{
    if (m_ready == ready)
        return;

    m_ready = ready;
    emit readyChanged(ready);
}
