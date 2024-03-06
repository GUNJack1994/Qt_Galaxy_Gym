#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>
#include "QTcpServer"
#include <QPointer>
#include <QTcpSocket>

class SocketServer : public QObject
{
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged)
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = 0);
    bool ready() const;

private:
    QTcpServer socket;
    QPointer<QTcpSocket> m_currentSocket;

    bool m_ready;

private slots:
    void onConnection();
    void onDisconnected();
signals:

    void readyChanged(bool ready);

public slots:
void setReady(bool ready);
void openDoor();
};

#endif // SOCKETSERVER_H
