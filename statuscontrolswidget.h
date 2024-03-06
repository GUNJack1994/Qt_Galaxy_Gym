#ifndef STATUSCONTROLSWIDGET_H
#define STATUSCONTROLSWIDGET_H

#include <QWidget>

namespace Ui {
class StatusControlsWidget;
}

class StatusControlsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusControlsWidget(QWidget *parent = 0);
    ~StatusControlsWidget();

public Q_SLOTS:
    void setSerialState(bool ok);
    void setBaseState(bool ok);

private:
    Ui::StatusControlsWidget *ui;
};

#endif // STATUSCONTROLSWIDGET_H
