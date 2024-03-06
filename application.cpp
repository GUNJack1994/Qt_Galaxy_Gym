#include "application.h"
#include <QFile>



Application::Application(int &argc, char **argv, int flags) :
    QApplication(argc, argv, flags)
{
    QFile styleFile("://style.css");
    if(styleFile.open(QFile::ReadOnly)) {
        setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    settings = new QSettings("rfidbasesettings.ini", QSettings::IniFormat);
    rfidManager = new RFIDManager;
    baseManager = new BaseManager;
}

