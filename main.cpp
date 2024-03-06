#include <QSettings>
#include <QTextCodec>
#include "mainwindow.h"
#include "rfidmanager.h"
#include "basemanager.h"
#include <QFile>
#include <QDebug>
#include <QTextCodec>
#include <QTcpServer>
#include "application.h"

int main(int argc, char *argv[])
{

    Application a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

//    QFile styleFile("://style.css");
//    if(styleFile.open(QFile::ReadOnly)) {
//        a.setStyleSheet(styleFile.readAll());
//        styleFile.close();
//    }

//    QSettings settings("rfidbasesettings.ini", QSettings::IniFormat);
//    //    qDebug() << "ustawienia" << settings.fileName();
//    RFIDManager rfidManager(&settings);

//    BaseManager baseManager(&settings);

    MainWindow w;
//    w.setStyleSheet(a.styleSheet());
    
    

    w.show();
//    baseManager.reconnect();
//    rfidManager.reconnect();

    QObject::connect(a.rfidManager, &RFIDManager::card, &w, &MainWindow::onCard);




    return a.exec();
}
