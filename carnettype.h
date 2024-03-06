#ifndef CARNETTYPE_H
#define CARNETTYPE_H
#include <QtGlobal>
#include <QString>
#include <QHash>

class CarnetType
{
public:
    CarnetType();
    int id;
    //int type;
    QString options;
    QString description;
    bool multi;
    QString colorString;
    bool hidden;
    QHash<QString, qreal> prices;
    bool enabled;
    bool isValid();
};

#endif // CARNETTYPE_H
