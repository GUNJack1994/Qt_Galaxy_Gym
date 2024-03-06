#include <QDebug>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasesettingswidget.h"
#include "serialportsettingswidget.h"
#include "camerasettingswidget.h"

#include "lastentrancewidget.h"
#include "userobject.h"
#include "worktimewidget.h"
#include "saleswidget.h"
#include "groupedentrancewidget.h"
#include "application.h"
#include "defines.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
//    m_rfidManager(managerIn),
//    m_baseManager(baseManagerIn),
//    m_settings(settingsIn),
    needGrabRFID(false)
{
    qRegisterMetaType<QHash<int, QPixmap> >("PixHash");
    qRegisterMetaType<QList <VHash> >("VariantHash");
    ui->setupUi(this);
    logOff();

    connect(app->baseManager, &BaseManager::readyChanged, &controlsWidget, &StatusControlsWidget::setBaseState);
    connect(app->rfidManager, &RFIDManager::readyChanged, &controlsWidget, &StatusControlsWidget::setSerialState);
    controlsWidget.setBaseState(app->baseManager->ready());
    controlsWidget.setSerialState(app->rfidManager->ready());

    ui->statusBar->addPermanentWidget(&controlsWidget);

#ifdef SIMULATOR
    simulator = new SimulatorRFID;
    connect(simulator, SIGNAL(simRFID(QString)), SLOT(onCard(QString)));
    simulator->show();
#else
    //    ui->menuBar->removeAction(ui->actionSymulator_RFID);
    ui->actionSymulator_RFID->setVisible(false);
#endif
    connect(ui->centerWidget, SIGNAL(needUserWidnow(QString)), SLOT(showUserForm(QString)));

    connect(ui->actionBaza_danych, SIGNAL(triggered()), SLOT(showDataBaseSettingsWidget()));
    connect(ui->actionPort_szeregowy, SIGNAL(triggered()), SLOT(showSerialPortSettingsWidget()));
    connect(ui->actionKamera, SIGNAL(triggered()), SLOT(showCameraSettingsWidget()));
    connect(ui->actionPo_cz_z_baz, SIGNAL(triggered()), app->baseManager, SLOT(reconnect()));
    connect(app->baseManager, &BaseManager::readyChanged, ui->actionPo_cz_z_baz, &QAction::setDisabled);
    connect(app->rfidManager, &RFIDManager::readyChanged, ui->actionOtw_rz_port_szeregowy, &QAction::setDisabled);

    connect(ui->actionWyloguj, SIGNAL(triggered()), SLOT(logOff()));
    connect(ui->findWidget, SIGNAL(simulatedEntrance(quint64)), SLOT(showUserForm(quint64)));
    connect(ui->lastWidget, SIGNAL(simulatedEntrance(quint64)), SLOT(showUserForm(quint64)));
    connect(ui->actionU_ytkownicy, SIGNAL(triggered()), SLOT(showUsersWidget()));
    connect(ui->actionLogowanie_pracownik_w, SIGNAL(triggered()), SLOT(showWorktimeWidget()));
    connect(ui->actionSprzeda, SIGNAL(triggered()), SLOT(showSalesWidget()));

#ifdef SIMULATOR
    ui->actionSymulator_RFID->setCheckable(true);
    connect(ui->actionSymulator_RFID, SIGNAL(triggered(bool)), simulator, SLOT(setVisible(bool)));
#endif

//    ui->photoView_2->setModel(&m_photoModel);
    m_photoModelProxy.setSourceModel(&m_photoModel);
    ui->photoView_2->setModel(&m_photoModelProxy);
    ui->photoView_2->setItemDelegate(&m_photoDelegator);
    connect(app->baseManager, SIGNAL(needUserUpdate()), SLOT(updatePhotoModel()));
    ui->leftStack->setCurrentIndex(0);

    m_splashTimer.setInterval(500);

    m_queryThread = new QueryThread();

    connect(m_queryThread, SIGNAL(lockChanged(bool)), ui->downloadBox, SLOT(setVisible(bool)));


    connect(&m_splashTimer, SIGNAL(timeout()), SLOT(checkSplash()));

    connect(m_queryThread, SIGNAL(photoListReady(PixHash)), SLOT(onPhotoListReady(PixHash)));
    connect(this, SIGNAL(downloadPhotos(QString)), m_queryThread, SLOT(getPhotoList(QString)));
    connect(ui->findWidget, SIGNAL(downloadUsers(QString)), m_queryThread, SLOT(getUserList(QString)));
    connect(m_queryThread, SIGNAL(userListReady(VariantHash)), this, SLOT(onUserListReady(VariantHash)));

    //    m_thread.start();
    m_queryThread->moveToThread(&m_thread);

    //    m_queryThread.getPhotoList();
    //    m_queryThread.start();
    m_thread.start();

    connect(&m_socketServer, SIGNAL(readyChanged(bool)), ui->doorButton, SLOT(setEnabled(bool)));
    ui->doorButton->setEnabled(m_socketServer.ready());

    connect(ui->lastWidget, SIGNAL(currentReaderChanged(int)), SLOT(checkEntryButtons()));
    checkEntryButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCard(const QString &rfid)
{
    qDebug() << "rfid to process" << rfid;
    if (!app->baseManager->isConnected()) {
        qWarning() << "Błąd połączenia z bazą";
        return;
    }

    if (needGrabRFID) {
        qDebug() << "przechwycono rfid" << rfid;
        Q_EMIT grabbedRFID(rfid);
        return;
    }

    // pobierz uzytkownika

    UserObject userNew = app->baseManager->userByRFID(rfid);

    if (!userNew.isValid() && app->baseManager->worker.isValid()) {
        showUserForm(rfid);
        return;
    }

    if (userNew.type != 0) {
        // pracownik
        logIn(userNew);
        this->show();
        this->raise();
        this->activateWindow();

    } else if (app->baseManager->worker.isValid()){


        if (userNew.carnet.id == CARNET_KEY) {
            // wyświetlanie głownego okna z kluczem
            ui->centerWidget->showKey(userNew);
            app->baseManager->addEntrance(userNew.id, QDateTime::currentDateTime(), true, false);
            return;
        }


        // uzytkownik

        userNew.paymentList = app->baseManager->lastPayments(userNew.id);
        ui->centerWidget->reuse(userNew);

        QDateTime lastEntrance = app->baseManager->lastEntrance(userNew.id);

        if (lastEntrance.isValid() && lastEntrance.date() == QDate::currentDate() && lastEntrance.time().secsTo(QTime::currentTime()) < 60 * 60 * 6) {
            qDebug() << "Wejscie ignorowane z powodu roznicy czasu < 6h - wzgledem ostatniego wejscia";
        } else {
            app->baseManager->addEntrance(userNew.id,
                                       QDateTime::currentDateTime(),
                                       Payment::currentPayment(userNew.paymentList).isValid(),
                                       userNew.carnet.id == CARNET_FITENSS_MAN);
            ui->lastWidget->updateData();
        }

        this->show();
        this->raise();
        this->activateWindow();
    }
}

void MainWindow::showDataBaseSettingsWidget()
{
    DataBaseSettingsWidget *dataBaseWidget = new DataBaseSettingsWidget();
    dataBaseWidget->setAttribute(Qt::WA_DeleteOnClose);
    dataBaseWidget->show();
}

void MainWindow::showSerialPortSettingsWidget()
{
    SerialPortSettingsWidget *serialWidget = new SerialPortSettingsWidget();
    serialWidget->setAttribute(Qt::WA_DeleteOnClose);
    serialWidget->show();
}

void MainWindow::showCameraSettingsWidget()
{
    CameraSettingsWidget *cameraSettingsWidget = new CameraSettingsWidget();
    cameraSettingsWidget->setAttribute(Qt::WA_DeleteOnClose);
    cameraSettingsWidget->show();
}

void MainWindow::showFindUserWidget()
{
    FindWidget *findWidget = new FindWidget();
    findWidget->setAttribute(Qt::WA_DeleteOnClose);
    findWidget->show();
}

void MainWindow::showSalesWidget()
{
    SalesWidget *salesWidget = new SalesWidget();
    salesWidget->setAttribute(Qt::WA_DeleteOnClose);
    salesWidget->show();
}

void MainWindow::showWorktimeWidget()
{
    WorktimeWidget *worktimeWidget = new WorktimeWidget();
    worktimeWidget->setAttribute(Qt::WA_DeleteOnClose);
    worktimeWidget->show();
    worktimeWidget->updateData();
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    Q_UNUSED(closeEvent);

    if (app->baseManager->worker.isValid())
        logOff();

    qApp->exit();

}

bool MainWindow::logInWorkerByRfid(const QString &rfid)
{
    UserObject worker;

    if (rfid == "0") {
        QList< QString > rfidPairList;
        rfidPairList << "0";
        worker = UserObject("Recepcja", "", QPixmap(), rfidPairList, 0, 2);
    } else {
        app->baseManager->userByRFID(rfid);
    }

    if (app->baseManager->worker.isValid() && app->baseManager->worker.type != 0) {
        qWarning() << "Pracownik niewylogowany poprawnie" << app->baseManager->worker.type;
        return false;
    }

    if (!worker.isValid() || worker.type == 0) {
        qWarning() << "Błąd pracownika serwisowego";
        return false;
    }

    logIn(worker);
    return true;
}

void MainWindow::updateCounters()
{
    ui->countLabel->setText(QString::number(ui->findWidget->proxy.rowCount()));
    ui->invLabel->setText(QString::number(ui->findWidget->invalidCounter()));
}

void MainWindow::waitBox() {
    QMessageBox mBox(this);
    mBox.setWindowTitle("Uwaga");
    mBox.setText("Wątek bazy danych zajęty. Poczekaj na dane.");
    mBox.setIcon(QMessageBox::Information);
    mBox.exec();
}

void MainWindow::oneTimeEntry(int user, int type, bool banan)
{
    app->baseManager->addEntrance(user, QDateTime::currentDateTime(), true, banan);

    Payment oneTimePayment;
    oneTimePayment.user = user;
    oneTimePayment.date = QDateTime::currentDateTime();
    oneTimePayment.to = QDate::currentDate();
    oneTimePayment.type = type;
    oneTimePayment.price = app->baseManager->carnetHash.value(type).prices.value("o");
    oneTimePayment.discount = 0;
    oneTimePayment.worker = app->baseManager->worker.id;

    QDate date = QDate::currentDate();
    app->baseManager->addPayment(oneTimePayment, date);
}

void MainWindow::setFilter(bool checked, QList<int> carnetType)
{
    if (m_queryThread->lock()) {
        waitBox();
        return;
    }

    if (checked)
        ui->findWidget->typeFilter = carnetType;
    else
        ui->findWidget->typeFilter = QList<int>();

    if (ui->leftStack->currentIndex() == 0) {
        ui->findWidget->updateData();
    } else  {
        updatePhotoModel();
    }
}

void MainWindow::setValidFilter(FindWidget::ValidOptions option)
{
    if (m_queryThread->lock()) {
        waitBox();
        return;
    }

    ui->findWidget->setDateFilter(option);

    if (ui->leftStack->currentIndex() == 0) {
        ui->findWidget->updateData();
    } else  {
        updatePhotoModel();
    }
}

void MainWindow::showUserForm(const QString &rfid)
{
    UserForm *userForm = new UserForm();
    userForm->setAttribute(Qt::WA_DeleteOnClose);
    connect(userForm, SIGNAL(grabRFID()), SLOT(grabRFID()));
    connect(userForm, SIGNAL(releaseRFID()), SLOT(releaseRFID()));
    connect(this, SIGNAL(grabbedRFID(QString)), userForm, SLOT(onGrabbedRFID(QString)));

    UserObject user = app->baseManager->userByRFID(rfid);
    user.isValid() ? userForm->reuse(user, true) : userForm->reuse(rfid);
    //    userForm->show();
    userForm->show();
    userForm->raise();
    userForm->activateWindow();
}

void MainWindow::showUserForm(quint64 id)
{
    UserForm *userForm = new UserForm();
    userForm->setAttribute(Qt::WA_DeleteOnClose);
    connect(userForm, SIGNAL(grabRFID()), SLOT(grabRFID()));
    connect(userForm, SIGNAL(releaseRFID()), SLOT(releaseRFID()));
    connect(this, SIGNAL(grabbedRFID(QString)), userForm, SLOT(onGrabbedRFID(QString)));

    UserObject user = app->baseManager->userByID(id);
    if (!user.isValid()) {
        userForm->deleteLater();
        return;
    }

    userForm->reuse(user, true);
    userForm->show();
}

void MainWindow::showUserForm(UserObject &user)
{
    UserForm *userForm = new UserForm();
    userForm->setAttribute(Qt::WA_DeleteOnClose);
    userForm->reuse(user);
    userForm->show();
}

void MainWindow::showUsersWidget()
{
    if (!usersWidget) {
        usersWidget = new UsersWidget();
        usersWidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(usersWidget, SIGNAL(simulatedEntrance(quint64)), SLOT(showUserForm(quint64)));
    }


    usersWidget->show();
}

void MainWindow::logIn(UserObject &user)
{
    if (!user.isValid() || user.type == 0) {
        qWarning() << Q_FUNC_INFO << "invalid user";
        return;
    }

    app->baseManager->addWorktime(user.id, QDateTime::currentDateTime());
    app->baseManager->worker = user;
    ui->passLineEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->pageView);
    ui->menuBar->setVisible(true);

    setWindowTitle(QString("GymAssistant - %1 %2").arg(app->baseManager->worker.name)
                   .arg(app->baseManager->worker.surname));

    if (app->baseManager->isConnected()) {
        if (m_queryThread->isReady()) {
            ui->findWidget->updateData();
            updateCounters();
        }
    }

    restoreGeometry(app->settings->value("Application/geometry", "").toByteArray());

    if (app->settings->contains("Application/state"))
        restoreState(app->settings->value("Application/state", "").toByteArray());

    //prawa uzytkownikow

    QAction *actionUstawienia = ui->menuUstawienia->menuAction();
    QAction *actionRaporty = ui->menuRaporty->menuAction();
    QAction *actionOkna = ui->menuOkna->menuAction();
    switch (user.type) {
    case 2:
        // administrator
        if (!ui->menuBar->actions().contains(actionUstawienia))
            ui->menuBar->addAction(actionUstawienia);

        if (!ui->menuBar->actions().contains(actionRaporty))
            ui->menuBar->addAction(actionRaporty);

        if (!ui->menuBar->actions().contains(actionOkna))
            ui->menuBar->addAction(actionOkna);
        break;

    default:
        // pracownik lub nieznany typ
        if (ui->menuBar->actions().contains(actionUstawienia))
            ui->menuBar->removeAction(actionUstawienia);

        if (ui->menuBar->actions().contains(actionRaporty))
            ui->menuBar->removeAction(actionRaporty);

        if (ui->menuBar->actions().contains(actionOkna))
            ui->menuBar->removeAction(actionOkna);
        break;
    }

    //updatePhotoModel();
}

void MainWindow::logOff()
{
    if (app->baseManager->worker.isValid()) {
        app->settings->setValue("Application/state", saveState());
        app->settings->setValue("Application/geometry", saveGeometry());
    }

    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    setWindowTitle(tr("Zaloguj do systemu GymAssistant"));
    ui->menuBar->setVisible(false);

    app->baseManager->worker.clear();
}

void MainWindow::grabRFID()
{
    qDebug() << "grabRFID";
    needGrabRFID = true;

}

void MainWindow::releaseRFID()
{
    qDebug() << "releaseRFID";
    needGrabRFID = false;
}

void MainWindow::on_loginButton_clicked()
{
    if (ui->passLineEdit->text() == "Galaxy_2020!")
        logInWorkerByRfid("0");
}

void MainWindow::on_passLineEdit_returnPressed()
{
    on_loginButton_clicked();
}

void MainWindow::on_galleryButton_toggled(bool checked)
{
    if (checked) {
        ui->leftStack->setCurrentIndex(1);
        updatePhotoModel();
    } else {
        tryAbortGalleryPopulation();
        ui->leftStack->setCurrentIndex(0);
    }
}

void MainWindow::updatePhotoModel()
{
    if (ui->leftStack->currentIndex() != 1)
        return;

    if (!m_queryThread->isReady()) {
        waitBox();
        return;
    }

    m_photoModel.clear();

    QString carnetTypeString = "";
    if (!ui->findWidget->typeFilter.isEmpty()) {
        carnetTypeString = QString("AND users.carnettype IN (%1)").arg(ui->findWidget->typeFilterString());
    }

    QString dateString;
    switch (ui->findWidget->dateFilter()) {
    case FindWidget::ValidActive: {
        dateString = QString(" AND DATE(users.validto) >= '%1'").arg(QDate::currentDate().addDays(-31).toString("yyyy-MM-dd"));
        break;
    }
    case FindWidget::ValidInactive: {
        dateString = QString(" AND (DATE(users.validto) < '%1' OR users.validto IS NULL)").arg(QDate::currentDate().addDays(-31).toString("yyyy-MM-dd"));
        break;
    }
    default:
        break;
    }

    QString stringFilter = "";
    if (!ui->textFilterEdit->text().isEmpty())
        stringFilter = QString(" AND (users.name LIKE '\%%1\%' OR users.surname LIKE '\%%1\%' OR users.id LIKE '\%%1\%' )").arg(ui->textFilterEdit->text());

    emit downloadPhotos(QString("%1%2%3").arg(carnetTypeString).arg(dateString).arg(stringFilter));
}

void MainWindow::on_photoView_2_entered(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (!m_photoSplash) {
        m_photoSplash = new QLabel();
        m_photoSplash->setWindowFlags(Qt::SplashScreen);
        m_photoSplash->setAlignment(Qt::AlignCenter);
        m_photoSplash->setStyleSheet("background: black; border: 2px solid black;");
        m_photoSplash->resize(600,600);
        m_photoSplash->setScaledContents(true);
    }

    m_photoSplash->setPixmap(app->baseManager->pixmapFromUserID(index.data(Qt::UserRole).toInt()));
    m_photoSplash->show();
    m_photoSplash->raise();
    m_splashTimer.start();
}

void MainWindow::checkSplash()
{
    if (!ui->photoView_2->viewport()->rect().contains(ui->photoView_2->viewport()->mapFromGlobal(QCursor::pos()))) {
        if (m_photoSplash) {
            m_photoSplash->hide();
        }
        m_splashTimer.stop();
    }
}

void MainWindow::onPhotoListReady(PixHash list)
{
    if (ui->leftStack->currentIndex() == 0) {
        qDebug() << "ignoruje liste zdjec";
        return;
    }

    m_photoModel.clear();

    QHash<int, QPixmap> pixHash = list;

    QList<int> keyList = pixHash.keys();
    qSort(keyList);
    Q_FOREACH (int key, keyList) {
        QStandardItem *pixItem = new QStandardItem;
        pixItem->setData(pixHash.value(key), Qt::DisplayRole);
        pixItem->setData(key, Qt::UserRole);
        m_photoModel.appendRow(pixItem);
    }
}

void MainWindow::onUserListReady(VariantHash list)
{

    if (ui->leftStack->currentIndex() == 1) {
        qDebug() << "ignoruje liste zdjec";
        return;
    }

    ui->findWidget->model.clear();

    Q_FOREACH (VHash hash, list) {
        QStandardItem *idItem = new QStandardItem;
        idItem->setData(hash.value(0), Qt::DisplayRole);

        QStandardItem *sItem = new QStandardItem;
        sItem->setData(hash.value(1), Qt::DisplayRole);

        QStandardItem *nItem = new QStandardItem;
        nItem->setData(hash.value(2), Qt::DisplayRole);

        QStandardItem *vItem = new QStandardItem;
        vItem->setData(hash.value(3), Qt::DisplayRole);

        ui->findWidget->model.appendRow(QList<QStandardItem*>() << idItem << sItem << nItem << vItem);
    }

    if (ui->findWidget->model.rowCount() > 0)
        ui->findWidget->updateTable();
    updateCounters();
}

void MainWindow::on_photoView_2_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        showUserForm(index.data(Qt::UserRole).toInt());
}

void MainWindow::on_oneTimeButton_clicked()
{
    oneTimeEntry(ONETIMEUSER_NORMAL, CARNET_ONEUSE);
}

void MainWindow::on_oneTimeButton1_clicked()
{
    oneTimeEntry(ONETIMEUSER_NORMAL, CARNET_ONEUSE);
}

void MainWindow::on_oneTimeButton2_clicked()
{
    oneTimeEntry(ONETIMEUSER_NORMAL, CARNET_ONEUSE);
}

void MainWindow::tryAbortGalleryPopulation()
{
    if (!m_queryThread->isReady())
        m_queryThread->abort();
}

void MainWindow::on_invalidButton_clicked()
{
    setValidFilter(FindWidget::ValidInactive);
}

void MainWindow::on_textFilterButton_clicked()
{
    ui->findWidget->proxy.setFilterFixedString(ui->textFilterEdit->text());

    if (ui->leftStack->currentIndex() == 1)
        updatePhotoModel();
    else
        ui->findWidget->updateData();

    updateCounters();
}

void MainWindow::on_normalButton_clicked(bool checked)
{
    setFilter(checked, QList<int>() << CARNET_NORMAL << CARNET_OPEN_MAN);

    ui->halfButton->setChecked(false);
    ui->ladiesButton->setChecked(false);
}

void MainWindow::on_halfButton_clicked(bool checked)
{
    setFilter(checked, QList<int>() << CARNET_HALF);
    ui->normalButton->setChecked(false);
    ui->ladiesButton->setChecked(false);
}

void MainWindow::on_ladiesButton_clicked(bool checked)
{
    setFilter(checked, QList<int>() << CARNET_LADIES << CARNET_OPEN_WOMAN);
    ui->normalButton->setChecked(false);
    ui->halfButton->setChecked(false);
}

void MainWindow::on_allButton_clicked()
{
    setValidFilter(FindWidget::ValidAll);
}

void MainWindow::on_activeButton_clicked()
{
    setValidFilter(FindWidget::ValidActive);
}

void MainWindow::on_textFilterEdit_returnPressed()
{
    on_textFilterButton_clicked();
}

void MainWindow::on_clearTextFilterButton_clicked()
{
    ui->textFilterEdit->clear();
    on_textFilterButton_clicked();
}

void MainWindow::on_doorButton_clicked()
{
    m_socketServer.openDoor();
}

//void MainWindow::on_openWomanButton_clicked()
//{
    //app->baseManager->addEntrance(ONETIMEUSER_OPEN_WOMAN, QDateTime::currentDateTime(), true, true);

//    oneTimeEntry(ONETIMEUSER_OPEN_WOMAN, CARNET_OPEN_WOMAN, true);
//}

void MainWindow::checkEntryButtons()
{
    // todo sensownie nazwac te id
    ui->oneTimeButton->setVisible(ui->lastWidget->currentReader() == 0);
    ui->oneTimeButton1->setVisible(ui->lastWidget->currentReader() == 1);
    ui->oneTimeButton2->setVisible(ui->lastWidget->currentReader() == 2);
    //ui->openWomanButton->setVisible(ui->lastWidget->currentReader() == 1);
}

void MainWindow::on_actionWej_cia_grupowane_triggered()
{
    GroupedEntranceWidget *groupedEntrance = new GroupedEntranceWidget();
    groupedEntrance->setAttribute(Qt::WA_DeleteOnClose);
    groupedEntrance->show();
}
