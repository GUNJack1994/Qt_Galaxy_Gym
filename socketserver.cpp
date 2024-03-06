#include "socketserver.h"
#include <QDebug>

SocketServer::SocketServer(QObject *parent) : QObject(parent)
{
    setReady(false);
    socket.listen(QHostAddress::Any, 3333);

    connect(&socket, SIGNAL(newConnection()), SLOT(onConnection()));

}

bool SocketServer::ready() const
{
    return m_ready;
}

void SocketServer::onConnection()
{
    if (!m_currentSocket) {
        m_currentSocket = socket.nextPendingConnection();
        qDebug() << "connection";
        setReady(true);
        connect(m_currentSocket.data(), SIGNAL(disconnected()), SLOT(onDisconnected()));
    } else {
        QTcpSocket *nextSocket =  socket.nextPendingConnection();
        nextSocket->close();
        nextSocket->deleteLater();

        qDebug() << "juz jest polaczenie";
    }
}

void SocketServer::onDisconnected()
{
    setReady(false);
    if (m_currentSocket) {
        qDebug() << "zabijam socket";
        m_currentSocket->deleteLater();
    }

}

void SocketServer::setReady(bool ready)
{
    if (m_ready == ready)
        return;

    m_ready = ready;
    emit readyChanged(ready);
}

void SocketServer::openDoor()
{
    if (m_currentSocket) {
        m_currentSocket->write("open");
    }
}

