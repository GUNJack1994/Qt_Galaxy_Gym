#include "userform.h"
#include "ui_userform.h"
#include <QDebug>
#include "takephotowidget.h"
#include <QMessageBox>
#include <QSqlError>
#include "application.h"
#include "keyform.h"

UserForm::UserForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserForm),
    m_currentState(EmptyState)
{
    ui->setupUi(this);
    ui->tableView->setModel(&m_paymentModel);
    ui->tableView->setItemDelegate(&paymentDelegate);
    ui->rfidView->setModel(&m_rfidModel);
    ui->familyView->setModel(&m_familyModel);
    initCombo();
    connect(app->baseManager, &BaseManager::needEntranceUpdate, this, &UserForm::updateEntranceData);
    connect(app->baseManager, &BaseManager::needPaymentUpdate, this, &UserForm::updatePaymentData);

    ui->promoCheckBox->setVisible(app->baseManager->worker.isValid() && app->baseManager->worker.type == 2);
}

UserForm::~UserForm()
{
    Q_EMIT releaseRFID();
    delete ui;
}

UserForm::WidgetStates UserForm::currentState() const
{
    return m_currentState;
}

void UserForm::initCombo()
{
    Q_FOREACH (CarnetType carnet, app->baseManager->carnetHash) {
        if (!carnet.hidden && carnet.enabled)
            ui->typeCombo->addItem(carnet.description, carnet.id);

        if (!carnet.multi && !carnet.hidden && carnet.enabled)
            ui->familyCombo->addItem(carnet.description, carnet.id);
    }

    ui->typeCombo->model()->sort(0);
    ui->familyCombo->model()->sort(0);

    ui->userTypeCombo->addItem(tr("Klient"), 0);
    ui->userTypeCombo->addItem(tr("Pracownik"), 1);
    ui->userTypeCombo->addItem(tr("Admin"), 2);
    ui->userTypeCombo->setCurrentIndex(0);

    ui->entranceView->setModel(&m_entranceModel);
    ui->entranceView->setItemDelegate(&m_entranceDelegate);
}

UserObject UserForm::userFromForm()
{
    UserObject newUser;
    newUser.name = ui->nameEdit->text();
    newUser.surname = ui->surnameEdit->text();
    newUser.telefon = ui->telefonEdit->text();
    newUser.carnet = ui->typeCombo->currentData().isValid() ? app->baseManager->carnetHash.value(ui->typeCombo->currentData().toInt()) : CarnetType();
    newUser.promotion = ui->promoCheckBox->isChecked() ? 1 : 0;

    // rfid
    for (int a = 0; a < m_rfidModel.rowCount(); ++a)
        newUser.rfidList.append(m_rfidModel.index(a, 0).data().toString());

    // rodzina
    for (int a = 0; a < m_familyModel.rowCount(); ++a)
        newUser.family << app->baseManager->carnetHash.value(m_familyModel.index(a,0).data(Qt::UserRole).toInt());

    newUser.photo = *ui->userLabel->pixmap();
    newUser.type = ui->userTypeCombo->currentIndex();

    return newUser;
}

void UserForm::updateEntranceData()
{
    QDate queryDate = ui->radio14->isChecked() ? QDate::currentDate().addDays(-14) : QDate::currentDate().addDays(-30);
    QString queryString = QString("SELECT date, valid FROM entrance WHERE user=%1 AND date>'%2' ORDER BY date DESC").arg(m_currentUser.id).arg(queryDate.toString("yyyy-MM-dd"));

    m_entranceModel.setQuery(queryString);

    if (m_entranceModel.lastError().type() != QSqlError::NoError) {
        app->baseManager->showErrorBox(m_entranceModel.lastError().text());
        return;
    }

    m_entranceModel.setHeaderData(0, Qt::Horizontal, tr("Data"), Qt::DisplayRole);
    ui->entranceView->hideColumn(1);
    ui->entranceView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void UserForm::updatePaymentData()
{
    if (!m_currentUser.isValid())
        return;

    m_paymentModel.setQuery(QString("SELECT payment.id, payment.date, payment.dateto, types.description, payment.price FROM payment INNER JOIN types ON payment.type=types.id WHERE user = %1 ORDER BY dateto DESC").arg(m_currentUser.id));

    if (m_paymentModel.lastError().type() != QSqlError::NoError) {
        app->baseManager->showErrorBox(m_paymentModel.lastError().text());
        return;
    }

    ui->tableView->hideColumn(0);
    m_paymentModel.setHeaderData(1, Qt::Horizontal, tr("Zaksięgowano"));
    m_paymentModel.setHeaderData(2, Qt::Horizontal, tr("Do"));
    m_paymentModel.setHeaderData(3, Qt::Horizontal, tr("Opis"));
    m_paymentModel.setHeaderData(4, Qt::Horizontal, tr("Cena"));

    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

}

void UserForm::checkRFIDInUse()
{
    bool inUse = app->baseManager->isRFIDInUse(ui->rfidEdit->text());
    ui->checkRFIDButton->setIcon(QIcon(inUse ? "://icons/task-attention.png" : "://icons/task-complete.png"));
}

void UserForm::onPixmapToSet(QPixmap pixmap, QWidget *widget)
{
    if (widget != ui->userLabel)
        return;

    if (pixmap.isNull()) {
        qWarning() << "empty pixmap";
        return;
    }

    ui->userLabel->setPixmap(pixmap);
}

void UserForm::onPhotoDone()
{
    if (tempPixmap.isNull())
        return;

    ui->userLabel->setPixmap(tempPixmap);
}

void UserForm::onExtend()
{
    if (!m_currentUser.isValid()) {
        qWarning() << "no current user";
        return;
    }

    if (!m_currentUser.validTo.isValid() || m_currentUser.validTo.daysTo(QDate::currentDate()) > 14) {
        dateFrom = QDate::currentDate();
    } else {
        dateFrom = m_currentUser.validTo;
    }

    on_typeCombo_currentIndexChanged(ui->typeCombo->currentIndex());
}

void UserForm::resetRFIDView()
{
    m_rfidModel.clear();
    m_rfidModel.setColumnCount(1);
    m_rfidModel.setHeaderData(0, Qt::Horizontal, QString(tr("RFID")), Qt::DisplayRole);
    ui->rfidView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void UserForm::reuse(UserObject &userObj, bool needPaymentUpdate, bool fresh)
{
    if (!app->baseManager || !app->settings) {
        qWarning() << "Error";
        return;
    }

    if (!userObj.isValid()) {
        setCurrentState(EmptyState);
        return;
    }

    m_currentUser = userObj;

    if (needPaymentUpdate && !fresh)
        m_currentUser.paymentList = app->baseManager->lastPayments(userObj.id);

    isActive = Payment::currentPayment(m_currentUser.paymentList).isValid();

    if (m_currentUser.type != 0 || isActive) {
        setCurrentState(ValidState);
    } else {
        setCurrentState(InvalidState);
    }

    ui->typeCombo->setCurrentIndex(ui->typeCombo->findData(m_currentUser.carnet.id));
    ui->familyCombo->setCurrentIndex(0);

    if (!fresh) {
        updatePaymentData();
        updateEntranceData();
    }

    ui->promoCheckBox->setChecked(userObj.promotion > 0);

    ui->userLabel->setPixmap(userObj.photo);
    ui->nameEdit->setText(userObj.name);
    ui->surnameEdit->setText(userObj.surname);
    ui->telefonEdit->setText(userObj.telefon);
    ui->idLabel->setText(QString::number(userObj.id));

    QStringList family;
    m_familyModel.clear();
    Q_FOREACH (CarnetType carnet, userObj.family) {
        family << QString::number(carnet.id);
        QStandardItem *familyItem = new QStandardItem(carnet.description);
        familyItem->setData(carnet.id, Qt::UserRole);
        m_familyModel.appendRow(familyItem);

    }

    resetRFIDView();

    Q_FOREACH (QString rfidString, userObj.rfidList)
        m_rfidModel.appendRow(new QStandardItem(rfidString));

    //ui->rfidLabel->setText(userObj.rfidList.join("; "));
    ui->userTypeCombo->setCurrentIndex(userObj.type);

    return;
}

void UserForm::reuse(const QString &rfid)
{
    if (!app->baseManager || !app->settings) {
        qWarning() << "Error";
        return;
    }

    resetRFIDView();
    m_tempUuid = rfid;

    m_rfidModel.appendRow(new QStandardItem(rfid));

    setCurrentState(EmptyState);
    return;
}

void UserForm::onGrabbedRFID(const QString &rfid)
{
    ui->rfidEdit->setText(rfid);
    ui->grabRFIDButton->setChecked(false);
}

void UserForm::setCurrentState(UserForm::WidgetStates currentState)
{
    //    if (m_currentState == currentState)
    //        return;

    m_currentState = currentState;

    ui->promoCheckBox->setEnabled(currentState == EditState);

    // zmiana stanu
    switch (m_currentState) {
    case EmptyState: {
        // klawisze glowne
        ui->addButton->setVisible(true);
        ui->addKeyButton->setVisible(app->baseManager->worker.type == 2);
        ui->cancelButton->setVisible(false);
        ui->takePhotoButton->setVisible(false);
        ui->extendButton->setVisible(false);
        ui->entranceButton->setVisible(false);
        ui->editButton->setVisible(false);

        // elementy ui
        ui->extendBox->setVisible(false);
        ui->paymentBox->setVisible(false);
        ui->rfidGroupBox->setVisible(false);
        ui->userGroupBox->setVisible(false);
        ui->groupBox->setVisible(false);
        ui->familyBox->setVisible(false);

        setWindowTitle(tr("Brak informacji"));
        break;
    }
    case AddState: {
        int freeId = app->baseManager->freeUserId();

        if (freeId > 0)
            ui->idLabel->setText(QString::number(freeId));

        // przyciski glowne
        ui->addButton->setVisible(false);
        ui->addKeyButton->setVisible(false);
        ui->cancelButton->setVisible(true);
        ui->takePhotoButton->setVisible(true);
        ui->takePhotoButton->setEnabled(!app->settings->value("Camera/name").toString().isEmpty());
        ui->cancelButton->setVisible(true);
        ui->extendButton->setVisible(false);
        ui->entranceButton->setVisible(false);
        ui->editButton->setVisible(false);

        // formularz
        ui->nameEdit->setEnabled(true);
        ui->surnameEdit->setEnabled(true);
        ui->telefonEdit->setEnabled(true);

        // elementy UI
        ui->extendBox->setVisible(false);
        ui->paymentBox->setVisible(false);
        ui->typeCombo->setEnabled(true);
        ui->rfidEditWidget->setVisible(true);
        ui->rfidGroupBox->setVisible(true);
        ui->rfidView->setEnabled(true);
        ui->userGroupBox->setVisible(true);
        ui->rmPaymentButton->setVisible(false);
        ui->groupBox->setVisible(false);
        ui->tableView->setSelectionMode(QTableView::NoSelection);

        // typ konta
        if (app->baseManager->worker.isValid() && app->baseManager->worker.type == 2) {
            ui->typeLabel->setVisible(true);
            ui->userTypeCombo->setVisible(true);
        } else {
            ui->typeLabel->setVisible(false);
            ui->userTypeCombo->setVisible(false);
        }

        ui->typeCombo->setCurrentIndex(ui->typeCombo->findData(1));

        setWindowTitle(tr("Dodaj użytkownika"));

        checkRFIDInUse();
        break;
    }

    case EditState:
        ui->nameEdit->setEnabled(true);
        ui->addKeyButton->setVisible(false);
        ui->surnameEdit->setEnabled(true);
        ui->telefonEdit->setEnabled(true);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(true);
        ui->takePhotoButton->setVisible(true);
        ui->takePhotoButton->setEnabled(!app->settings->value("Camera/name").toString().isEmpty());
        //ui->okButton->setVisible(true);
        ui->addButton->setVisible(false);
        ui->extendButton->setVisible(false);
        ui->entranceButton->setVisible(false);
        ui->editButton->setVisible(false);
        setWindowTitle(tr("Edytuj użytkownika"));
        ui->extendBox->setVisible(false);
        ui->paymentBox->setVisible(false);
        if (app->baseManager->worker.isValid() && app->baseManager->worker.type == 2) {
            //        ui->workerGroupBox->setVisible(true);
            ui->typeLabel->setVisible(true);
            ui->userTypeCombo->setVisible(true);
        }
        ui->rfidEditWidget->setVisible(true);
        ui->rfidView->setVisible(true);
        ui->rfidView->setEnabled(true);
        ui->rfidGroupBox->setVisible(true);
        ui->userGroupBox->setVisible(true);
        ui->typeCombo->setEnabled(true);
        ui->rmPaymentButton->setVisible(false);
        ui->groupBox->setVisible(false);
        ui->tableView->setSelectionMode(QTableView::NoSelection);
        checkRFIDInUse();
        break;

    case ExtendedState:
        ui->addKeyButton->setVisible(false);
        ui->nameEdit->setEnabled(false);
        ui->surnameEdit->setEnabled(false);
        ui->telefonEdit->setEnabled(false);
        //ui->dateEdit->setEnabled(false);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->takePhotoButton->setVisible(false);
        //ui->okButton->setVisible(true);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(true);
        ui->extendButton->setVisible(false);
        ui->entranceButton->setVisible(false);
        ui->editButton->setVisible(false);
        setWindowTitle(tr("Przedłuż ważność"));
        ui->extendBox->setVisible(true);
        ui->paymentBox->setVisible(true);
        //    ui->typeCombo->setEnabled(true);
        //    ui->workerGroupBox->setVisible(false);
        ui->typeCombo->setEnabled(false);
        ui->typeLabel->setVisible(false);
        ui->userTypeCombo->setVisible(false);
        ui->rfidEditWidget->setVisible(false);
        ui->rfidView->setVisible(true);
        ui->rfidView->setEnabled(false);
        ui->rfidGroupBox->setVisible(false);
        ui->userGroupBox->setVisible(true);
        ui->rmPaymentButton->setVisible(true);
        ui->groupBox->setVisible(true);
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

        m_currentState = ExtendedState;
        onExtend();
        break;

    case InvalidState:
        ui->addKeyButton->setVisible(false);
        ui->nameEdit->setEnabled(false);
        ui->surnameEdit->setEnabled(false);
        ui->telefonEdit->setEnabled(false);
        //ui->dateEdit->setEnabled(false);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->takePhotoButton->setVisible(false);
        //ui->okButton->setVisible(true);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->extendButton->setVisible(true);
        ui->entranceButton->setVisible(true);
        ui->editButton->setVisible(true);
        setWindowTitle(tr("Konto nieopłacone"));
        ui->extendBox->setVisible(false);
        ui->paymentBox->setVisible(true);
        //    ui->workerGroupBox->setVisible(false);
        ui->typeLabel->setVisible(false);
        ui->userTypeCombo->setVisible(false);
        ui->rfidEditWidget->setVisible(false);
        ui->rfidView->setVisible(true);
        ui->rfidView->setEnabled(false);
        ui->rfidGroupBox->setVisible(true);
        ui->userGroupBox->setVisible(true);
        ui->typeCombo->setEnabled(false);
        ui->rmPaymentButton->setVisible(false);
        ui->groupBox->setVisible(true);
        ui->tableView->setSelectionMode(QTableView::NoSelection);
        break;

    case ValidState:
        ui->addKeyButton->setVisible(false);
        ui->nameEdit->setEnabled(false);
        ui->surnameEdit->setEnabled(false);
        ui->telefonEdit->setEnabled(false);
        //ui->dateEdit->setEnabled(false);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->takePhotoButton->setVisible(false);
        //ui->okButton->setVisible(true);
        ui->addButton->setVisible(false);
        ui->cancelButton->setVisible(false);
        ui->extendButton->setVisible(m_currentUser.type == 0);
        ui->entranceButton->setVisible(m_currentUser.type == 0);
        ui->editButton->setVisible(true);
        setWindowTitle(tr("Użytkownik"));
        ui->extendBox->setVisible(false);
        ui->paymentBox->setVisible(m_currentUser.type == 0);
        //    ui->workerGroupBox->setVisible(false);
        ui->typeCombo->setEnabled(false);
        ui->typeLabel->setVisible(false);
        ui->userTypeCombo->setVisible(false);
        ui->rfidEditWidget->setVisible(false);
        ui->rfidView->setVisible(true);
        ui->rfidView->setEnabled(false);
        ui->rfidGroupBox->setVisible(true);
        ui->userGroupBox->setVisible(true);
        ui->rmPaymentButton->setVisible(false);
        ui->groupBox->setVisible(true);
        ui->tableView->setSelectionMode(QTableView::NoSelection);
        m_currentState = ValidState;
        break;

    default:
        break;
    }

    // dowanie czlownkow rodziny
    bool elementVisibile = false;
    switch (m_currentState) {
    case EditState:
        elementVisibile = app->baseManager->carnetHash.value(ui->typeCombo->currentData().toInt()).multi;
        break;
    default:
        break;
    }
    ui->familyAdd->setVisible(elementVisibile);
    ui->familiRm->setVisible(elementVisibile);
    ui->familyCombo->setVisible(elementVisibile);
    ui->familyAdd->setVisible(elementVisibile);

    emit currentStateChanged(currentState);
}

void UserForm::on_okButton_clicked()
{
    QMessageBox *mBox = new QMessageBox;
    mBox->setAttribute(Qt::WA_DeleteOnClose);

    QString errorString;

    switch (m_currentState) {
    case ValidState:
    case EmptyState:
    case InvalidState:
        close();
        break;

    case AddState: {
        UserObject newUser = userFromForm();

        bool isNum;
        int userId = ui->idLabel->text().toInt(&isNum);

        qlonglong newId = (isNum && userId > 0) ? userId : -1;
        if (app->baseManager->addUser(newUser, newId)) {
            app->baseManager->emitNUU();
            newUser.id = newId;
            m_currentUser = newUser;
            reuse(newUser, false, true);
        } else {
            mBox->setWindowTitle(tr("Nie dodano użytkownika"));
            mBox->setText(tr("Proszę sprawdzić poprawność danych w formularzu"));
            mBox->show();
        }
        break;
    }

    case EditState: {
        UserObject newUser = userFromForm();
        newUser.id = m_currentUser.id;
        newUser.validTo = m_currentUser.validTo;

        if (app->baseManager->carnetHash.value(ui->typeCombo->currentData().toInt()).multi && m_familyModel.rowCount() < 2) {
            errorString.append(tr("Nieprawidłowa ilość członków rodziny."));
            break;
        }

        // platnosci
        newUser.paymentList = app->baseManager->lastPayments(newUser.id);

        if (app->baseManager->updateUser(newUser)) {
            m_currentUser = newUser;
            reuse(newUser);
        } else {
            mBox->setWindowTitle(tr("Nie zapisano informacji."));
            mBox->setText(tr("Proszę sprawdzić poprawność danych w formularzu"));
            mBox->show();
        }
        break;
    }

    case ExtendedState: {
        Payment payment;
        payment.user = m_currentUser.id;
        payment.date = QDateTime::currentDateTime();
        payment.to = ui->toEdit->date();
        payment.type = ui->typeCombo->currentData().toInt();
        payment.worker = app->baseManager->worker.id;
        payment.discount = 0;
        payment.promotion = m_currentUser.promotion > 0 ? 10 : 0;

        qreal price = 0;
        QString pricePrefix;

        if (ui->button1m->isChecked())
            pricePrefix = "m1";
        else if (ui->button3m->isChecked())
            pricePrefix = "m3";
        else if (ui->button6m->isChecked())
            pricePrefix = "m6";
        else if (ui->button14d->isChecked())
            pricePrefix = "d14";

        if (!app->baseManager->carnetHash.contains(ui->typeCombo->currentData().toInt())) {
            errorString.append(tr("Nieznany typ karentu."));
            break;
        }

        CarnetType *currentCarnet = &app->baseManager->carnetHash[ui->typeCombo->currentData().toInt()];

        QString bill;

        bill.append(QString(tr("Karnet %1\n\n")).arg(currentCarnet->description));

        // rachunek

        if (currentCarnet->multi && m_familyModel.rowCount() < 2) {
            errorString.append(tr("Nieprawidłowa ilość kart RFID dla konta rodzinnego."));
            break;
        } else if (currentCarnet->multi) {
            // rodzinny
            for (int a = 0; a < m_familyModel.rowCount(); ++a) {
                price += app->baseManager->carnetHash.value(m_familyModel.index(a,0).data(Qt::UserRole).toInt()).prices.value(pricePrefix);
                bill.append(QString::number(app->baseManager->carnetHash.value(m_familyModel.index(a,0).data(Qt::UserRole).toInt()).prices.value(pricePrefix)));
                bill.append(" ");
                bill.append(app->baseManager->carnetHash.value(m_familyModel.index(a,0).data(Qt::UserRole).toInt()).description);
                bill.append("\n");
            }

            qreal discount = m_familyModel.rowCount() * currentCarnet->prices.value(pricePrefix);

            price += discount;
            price -= payment.promotion;


            bill.append(QString(tr("\n%1 zniżka \n")).arg(QString::number(discount)));
            if (payment.promotion > 0)
                bill.append(QString(tr("\n%1 promocja \n")).arg(QString::number(payment.promotion)));

            bill.append(QString(tr("=====\nRazem: %1")).arg(QString::number(price)));

        } else {
            //normalny
            price = currentCarnet->prices.value(pricePrefix);
            if (payment.promotion > 0)
                bill.append(QString(tr("\n%1 promocja \n")).arg(QString::number(payment.promotion)));
            bill.append(QString(tr("=====\nRazem: %1")).arg(QString::number(price)));
        }

        if (app->baseManager->carnetHash.value(ui->typeCombo->currentData().toInt()).prices.contains(pricePrefix)) {
            payment.price = price;
        } else {
            payment.price = 0;
        }

        payment.ticketTypeDescription = currentCarnet->description;

        qDebug() << "bill" << bill;

        QDate validTo;
        if (errorString.isEmpty() && app->baseManager->addPayment(payment, validTo)) {

            UserObject user = app->baseManager->userByID(m_currentUser.id);
            reuse(user, true);

            // todo MASAKRA!!!!!!!!!!!!!!!
            app->baseManager->emitNUU();

        } else {
            mBox->setWindowTitle(tr("Nie dokonano płatnośći"));
            mBox->setText(tr("Proszę sprawdzić poprawność danych w formularzu"));
            mBox->show();
        }
        break;
    }
    }


    if (!errorString.isEmpty()) {
        mBox->setWindowTitle(tr("Błąd danych formularza"));
        mBox->setText(errorString);
        mBox->show();
    }

    if (!mBox->isVisible())
        mBox->deleteLater();
}

void UserForm::on_takePhotoButton_clicked()
{
    TakePhotoWidget *takePhotoWidget = new TakePhotoWidget(tempPixmap);
    takePhotoWidget->setAttribute(Qt::WA_DeleteOnClose);
    takePhotoWidget->show();
    tempPixmap = QPixmap();
    connect(takePhotoWidget, SIGNAL(photoDone()), SLOT(onPhotoDone()));
}

void UserForm::on_extendButton_clicked()
{
    setCurrentState(ExtendedState);
}

void UserForm::on_entranceButton_clicked()
{
    QDateTime lastEntrance = app->baseManager->lastEntrance(m_currentUser.id);

    if (lastEntrance.isValid() && lastEntrance.date() == QDate::currentDate() && lastEntrance.time().secsTo(QTime::currentTime()) < 60 * 60 * 6) {
        qDebug() << "Wejscie ignorowane z powodu roznicy czasu < 6h - wzgledem ostatniego wejscia";
        return;
    }

    app->baseManager->addEntrance(m_currentUser.id, QDateTime::currentDateTime(), isActive);
}

void UserForm::on_cancelButton_clicked()
{
    if (m_currentState == ExtendedState
            || m_currentState == AddState
            || m_currentState == EditState) {
        reuse(m_currentUser);
    }
}

void UserForm::on_editButton_clicked()
{
    setCurrentState(EditState);
}

void UserForm::on_addButton_clicked()
{
    setCurrentState(AddState);
}

void UserForm::on_typeCombo_currentIndexChanged(int index)
{
    QString validity = app->baseManager->carnetHash.value(ui->typeCombo->itemData(index).toInt()).options;

    QStringList options = validity.split(";");

    ui->button1m->setVisible(false);
    ui->button3m->setVisible(false);
    ui->button6m->setVisible(false);
    ui->button14d->setVisible(false);

    bool selected = false;
    Q_FOREACH (QString option, options) {
        if (option.contains("m1")) {
            ui->button1m->setVisible(true);
            if (!selected) {
                ui->button1m->click();
                selected = true;
            }
        } else if (option.contains("m3")) {
            ui->button3m->setVisible(true);
            if (!selected) {
                ui->button3m->click();
                selected = true;
            }
        } else if (option.contains("m6")) {
            ui->button6m->setVisible(true);
            if (!selected) {
                ui->button6m->click();
                selected = true;
            }
        } else if (option.contains("d14")) {
            ui->button14d->setVisible(true);
            if (!selected) {
                ui->button14d->click();
                selected = true;
            }
        }
    }

    if (app->baseManager->carnetHash.value(ui->typeCombo->itemData(index).toInt()).multi) {
        ui->familyBox->setVisible(true);
    } else
        ui->familyBox->setVisible(false);


    if (m_currentState == EditState && app->baseManager->carnetHash.value(ui->typeCombo->itemData(index).toInt()).multi) {
        ui->familiRm->setVisible(true);
        ui->familyAdd->setVisible(true);
        ui->familyCombo->setVisible(true);
    }
}

void UserForm::on_addRFIDButton_clicked()
{
    // walidacja
    if (ui->rfidEdit->text().isEmpty()) {
        QMessageBox *mBox = new QMessageBox;
        mBox->setAttribute(Qt::WA_DeleteOnClose);
        mBox->setWindowTitle(tr("Nie można dodać karty RFID."));
        mBox->setText(tr("Sprawdź poprawność danych w formularzu."));
        mBox->setIcon(QMessageBox::Warning);
        mBox->setWindowIcon(QIcon("://icons/Industry-Rfid-Tag-icon.png"));
        mBox->setStandardButtons(QMessageBox::Ok);
        mBox->setDefaultButton(QMessageBox::Ok);
        mBox->show();
        return;
    }


    QStandardItem *rfidItem = new QStandardItem(ui->rfidEdit->text());

    m_rfidModel.appendRow(rfidItem);
}

void UserForm::on_grabRFIDButton_toggled(bool checked)
{
    if (checked)
        Q_EMIT grabRFID();
    else
        Q_EMIT releaseRFID();

    ui->rfidEdit->setEnabled(!checked);
}

void UserForm::on_checkRFIDButton_clicked()
{
    checkRFIDInUse();
}

void UserForm::on_rmRFIDButton_clicked()
{
    if (!ui->rfidView->currentIndex().isValid())
        return;

    m_rfidModel.removeRow(ui->rfidView->currentIndex().row());
}

void UserForm::on_button14d_clicked()
{
    ui->toEdit->setDate(dateFrom.addDays(14));
}

void UserForm::on_button1m_clicked()
{
    ui->toEdit->setDate(dateFrom.addMonths(1));
}

void UserForm::on_button3m_clicked()
{
    ui->toEdit->setDate(dateFrom.addMonths(3));
}

void UserForm::on_button6m_clicked()
{
    ui->toEdit->setDate(dateFrom.addMonths(6));
}

void UserForm::on_rmPaymentButton_clicked()
{
    if (!ui->tableView->currentIndex().isValid())
        return;

    QDate validTo;
    QMessageBox *mBox = new QMessageBox;
    mBox->setAttribute(Qt::WA_DeleteOnClose);
    mBox->setStandardButtons(QMessageBox::Ok);
    mBox->setDefaultButton(QMessageBox::Ok);
    if (app->baseManager->rmPayment(m_paymentModel.index(ui->tableView->currentIndex().row(), 0).data().toUInt(), m_currentUser.id, validTo)) {
        m_currentUser.validTo = validTo;
        mBox->setWindowTitle(tr("Usunięto płatność."));
        mBox->setText(tr("Płatność została pomyślnie usunięta."));
        mBox->setIcon(QMessageBox::Warning);
    } else {
        mBox->setWindowTitle(tr("Nie można usunąć płatności."));
        mBox->setText(tr("Sprawdź poprawność danych w bazie."));
        mBox->setIcon(QMessageBox::Warning);
    }
    mBox->show();
}

void UserForm::on_familyAdd_clicked()
{
    CarnetType *currentCarnet = &app->baseManager->carnetHash[ui->familyCombo->currentData().toInt()];
    QStandardItem *familyItem = new QStandardItem(currentCarnet->description);
    familyItem->setData(currentCarnet->id, Qt::UserRole);
    m_familyModel.appendRow(familyItem);
}

void UserForm::on_familiRm_clicked()
{
    if (ui->familyView->currentIndex().isValid())
        m_familyModel.removeRow(ui->familyView->currentIndex().row());
}

void UserForm::on_radio14_clicked()
{
    updateEntranceData();
}

void UserForm::on_radio30_clicked()
{
    updateEntranceData();
}

void UserForm::on_addKeyButton_clicked()
{
  KeyForm *keyForm = new KeyForm(m_tempUuid, parentWidget());
  keyForm->show();
  close();
}
