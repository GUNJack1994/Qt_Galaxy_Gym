#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "rfidmanager.h"
#include "basemanager.h"
#include <QSettings>

#define app (static_cast<Application *>(Application::instance()))

class Application : public QApplication
{
public:
    Application(int &argc, char **argv, int flags = ApplicationFlags);
    RFIDManager *rfidManager;
    BaseManager *baseManager;
    QSettings *settings;
};

#endif // APPLICATION_H
