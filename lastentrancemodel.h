#ifndef LASTENTRANCEMODEL_H
#define LASTENTRANCEMODEL_H

#include <QSqlQueryModel>

class LastEntranceModel : public QSqlQueryModel
{
public:
    LastEntranceModel();

public:
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // LASTENTRANCEMODEL_H
