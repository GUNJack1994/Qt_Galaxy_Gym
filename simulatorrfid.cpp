#include "simulatorrfid.h"
#include "ui_simulatorrfid.h"

SimulatorRFID::SimulatorRFID(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulatorRFID)
{
    ui->setupUi(this);
}

SimulatorRFID::~SimulatorRFID()
{
    delete ui;
}

void SimulatorRFID::on_pushButton_clicked()
{
    Q_EMIT simRFID(ui->lineEdit->text());
}
