#include "keyform.h"
#include "ui_keyform.h"
#include "application.h"
#include <QMessageBox>

KeyForm::KeyForm(const QString &uuid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyForm),
    m_uuid(uuid)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet(qApp->styleSheet());


    ui->rfidLabel->setText(uuid);
    ui->lineEdit->setText(uuid);
}

KeyForm::~KeyForm()
{
    delete ui;
}

void KeyForm::on_pushButton_clicked()
{
    UserObject newUser;
    newUser.name = ui->lineEdit->text();
    newUser.surname = "KLUCZ";
    newUser.carnet = app->baseManager->carnetHash.value(10);
    newUser.rfidList.append(m_uuid);
    newUser.type = 0;
    qlonglong newId = -1;
    bool isOK = app->baseManager->addUser(newUser, newId);
    QMessageBox *mBox = new QMessageBox();
    mBox->setAttribute(Qt::WA_DeleteOnClose);
    mBox->setWindowTitle("Dodawanie klucza");
    if (isOK) {
        mBox->setText("Pomyślnie dodano klucz.");
        mBox->show();
        close();
    } else {
        mBox->setText("Nie można dodać klucza. Sprawdź czy klucz nie jest już zarejestrowany.");
        mBox->show();
    }
}
