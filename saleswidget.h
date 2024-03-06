#ifndef SALESWIDGET_H
#define SALESWIDGET_H

#include <QWidget>
#include "QSqlQueryModel"

namespace Ui {
class SalesWidget;
}

class SalesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SalesWidget(QWidget *parent = 0);
    ~SalesWidget();

private slots:
    void on_dateEdit_dateChanged(const QDate &date);

    void on_dayRadio_toggled(bool checked);

    void on_monthRadio_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::SalesWidget *ui;
    QSqlQueryModel m_salesModel;
    void updateData();
};

#endif // SALESWIDGET_H
