#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include "rowfilter.h"
#include "finduserdelegate.h"
#include <QStandardItemModel>

namespace Ui {
class FindWidget;
}

class FindWidget : public QWidget
{
    Q_OBJECT

public:
    enum ValidOptions {
        ValidAll = 0,
        ValidActive = 1,
        ValidInactive = 2
    };

    explicit FindWidget(QWidget *parent = 0);
    ~FindWidget();
//    void setTpyeFilter(int type, bool needUpdateData = true);
    void setDateFilter(ValidOptions option);
//    int typeFilter();
    ValidOptions dateFilter();
    RowFilter proxy;
    QStandardItemModel model;
    void updateTable();
    QList<int> typeFilter;
    QString typeFilterString();

public Q_SLOTS:
    void updateData();
    int invalidCounter();

private Q_SLOTS:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void onStatusChange(bool connected);

private:
    Ui::FindWidget *ui;
    FindUserDelegate m_delegate;
    ValidOptions m_validFilter;

Q_SIGNALS:
    void simulatedEntrance(quint64 id);
    void downloadUsers(const QString &params = "");
};

#endif // FINDWIDGET_H
