#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>

namespace Ui {
class UsersWidget;
}

class UsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = 0);
    ~UsersWidget();

private slots:
    void on_deleteButton_clicked();

private:
    Ui::UsersWidget *ui;
    QSqlQueryModel m_model;

private Q_SLOTS:
    void updateData();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_comboBox_currentIndexChanged(int index);
    void on_workerRadio_toggled(bool checked);
    void on_clientRadio_toggled(bool checked);

Q_SIGNALS:
    void simulatedEntrance(quint64);
};

#endif // USERSWIDGET_H
