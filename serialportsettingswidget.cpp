#include "serialportsettingswidget.h"
#include "ui_serialportsettingswidget.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include "application.h"

SerialPortSettingsWidget::SerialPortSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPortSettingsWidget)
{
    ui->setupUi(this);
    ui->portNameComboBox->addItem("");
    Q_FOREACH(QSerialPortInfo info, QSerialPortInfo::availablePorts())
        ui->portNameComboBox->addItem(info.portName());

    ui->baudRateBox->addItem("");
    Q_FOREACH(quint32 speed, QSerialPortInfo::standardBaudRates())
        ui->baudRateBox->addItem(QString::number(speed, 10));

    ui->parityBox->addItem("", QSerialPort::UnknownParity);
    ui->parityBox->addItem("NoParity", QSerialPort::NoParity);
    ui->parityBox->addItem("EvenParity", QSerialPort::EvenParity);
    ui->parityBox->addItem("OddParity", QSerialPort::OddParity);
    ui->parityBox->addItem("SpaceParity", QSerialPort::SpaceParity);
    ui->parityBox->addItem("MarkParity", QSerialPort::MarkParity);

    ui->stopBox->addItem("", QSerialPort::UnknownStopBits);
    ui->stopBox->addItem("OneStop", QSerialPort::OneStop);
    ui->stopBox->addItem("OneAndHalfStop", QSerialPort::OneAndHalfStop);
    ui->stopBox->addItem("TwoStop", QSerialPort::TwoStop);

    ui->dataBitsBox->addItem("", QSerialPort::UnknownDataBits);
    ui->dataBitsBox->addItem("5", QSerialPort::Data5);
    ui->dataBitsBox->addItem("6", QSerialPort::Data6);
    ui->dataBitsBox->addItem("7", QSerialPort::Data7);
    ui->dataBitsBox->addItem("8", QSerialPort::Data8);

    ui->portNameComboBox->setCurrentText(app->settings->value("SerialPort/name", "").toString());
    ui->baudRateBox->setCurrentText(app->settings->value("SerialPort/baudrate", "").toString());
    ui->dataBitsBox->setCurrentIndex(ui->dataBitsBox->findData(static_cast<QSerialPort::DataBits>(app->settings->value("SerialPort/databits", -1).toInt())));
    ui->parityBox->setCurrentIndex(ui->parityBox->findData(static_cast<QSerialPort::Parity>(app->settings->value("SerialPort/parity", -1).toInt())));
    ui->stopBox->setCurrentIndex(ui->stopBox->findData(static_cast<QSerialPort::StopBits>(app->settings->value("SerialPort/stopbits", -1).toInt())));

}

SerialPortSettingsWidget::~SerialPortSettingsWidget()
{
    delete ui;
}

void SerialPortSettingsWidget::on_buttonBox_clicked(QAbstractButton *aButton)
{
    switch (ui->buttonBox->standardButton(aButton)) {

    case QDialogButtonBox::Apply:
        saveData();
        app->rfidManager->reconnect();
        break;
    case QDialogButtonBox::Cancel:
        close();
        break;
    case QDialogButtonBox::Ok:
        saveData();
        app->rfidManager->reconnect();
        close();
        break;
    default:
        break;
    }
}

void SerialPortSettingsWidget::saveData()
{
    app->settings->setValue("SerialPort/name", ui->portNameComboBox->currentText());
    app->settings->setValue("SerialPort/baudrate", ui->baudRateBox->currentText());
    app->settings->setValue("SerialPort/databits", ui->dataBitsBox->currentData());
    app->settings->setValue("SerialPort/parity", ui->parityBox->currentData());
    app->settings->setValue("SerialPort/stopbits", ui->stopBox->currentData());
}
