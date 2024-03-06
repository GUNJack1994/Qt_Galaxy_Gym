#include "statuscontrolswidget.h"
#include "ui_statuscontrolswidget.h"

StatusControlsWidget::StatusControlsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusControlsWidget)
{
    ui->setupUi(this);
}

StatusControlsWidget::~StatusControlsWidget()
{
    delete ui;
}

void StatusControlsWidget::setSerialState(bool ok)
{
    ui->rfidBox->setChecked(ok);
}

void StatusControlsWidget::setBaseState(bool ok)
{
    ui->baseBox->setChecked(ok);
}
