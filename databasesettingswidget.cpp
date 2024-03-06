#include "databasesettingswidget.h"
#include "ui_databasesettingswidget.h"
#include <QDebug>
#include "application.h"

DataBaseSettingsWidget::DataBaseSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataBaseSettingsWidget)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonBoxClicked(QAbstractButton*)));
    updateData();
}

DataBaseSettingsWidget::~DataBaseSettingsWidget()
{
    delete ui;
}

void DataBaseSettingsWidget::updateData()
{
    ui->addressLineEdit->setText(app->settings->value("DataBase/address", QString("")).toString());
    ui->loginLineEdit->setText(app->settings->value("DataBase/login", QString("")).toString());
    ui->passLineEdit->setText(app->settings->value("DataBase/pass", QString("")).toString());
    ui->baseLineEdit->setText(app->settings->value("DataBase/base", QString("")).toString());
}

void DataBaseSettingsWidget::saveData()
{
    app->settings->setValue("DataBase/address", ui->addressLineEdit->text());
    app->settings->setValue("DataBase/login", ui->loginLineEdit->text());
    app->settings->setValue("DataBase/pass", ui->passLineEdit->text());
    app->settings->setValue("DataBase/base", ui->baseLineEdit->text());
}

void DataBaseSettingsWidget::on_buttonBox_rejected()
{
    //    close();
}

void DataBaseSettingsWidget::onButtonBoxClicked(QAbstractButton *aButton)
{
    switch (ui->buttonBox->standardButton(aButton)) {

    case QDialogButtonBox::Apply:
        saveData();
        app->baseManager->reconnect();
        break;
    case QDialogButtonBox::Cancel:
        close();
        break;
    case QDialogButtonBox::Ok:
        saveData();
        app->baseManager->reconnect();
        close();
        break;
    default:
        break;
    }
}
