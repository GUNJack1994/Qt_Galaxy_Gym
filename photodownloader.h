#ifndef PHOTODOWNLOADER_H
#define PHOTODOWNLOADER_H

#include <QObject>
//#include "querythread.h"

class PhotoDownloader : public QObject
{
    Q_OBJECT

public:
    explicit PhotoDownloader(QObject *parent = 0);

private:


signals:

public slots:
};

#endif // PHOTODOWNLOADER_H
