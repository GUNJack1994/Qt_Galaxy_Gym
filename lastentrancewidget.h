#ifndef LASTENTRANCEWIDGET_H
#define LASTENTRANCEWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include "entrancedelegate.h"
#include "lastentrancemodel.h"

namespace Ui {
class LastEntranceWidget;
}

class LastEntranceWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentReader READ currentReader WRITE setCurrentReader NOTIFY currentReaderChanged)

public:
    explicit LastEntranceWidget(QWidget *parent = 0);
    ~LastEntranceWidget();

    int currentReader() const;

public Q_SLOTS:
    void updateData();

    void setCurrentReader(int currentReader);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_reader0button_clicked();

    void on_reader1button_clicked();

    void on_reader2button_clicked();

private:
    Ui::LastEntranceWidget *ui;
    LastEntranceModel m_model;
    EntranceDelegate delegate;
//    int m_currentReader;

    int m_currentReader;

Q_SIGNALS:
    void simulatedEntrance(quint64);

    void currentReaderChanged(int currentReader);
};

#endif // LASTENTRANCEWIDGET_H
