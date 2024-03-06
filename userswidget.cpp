#include "userswidget.h"
#include "ui_userswidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QHeaderView>
#include "application.h"
#include "defines.h"

UsersWidget::UsersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsersWidget)
{
    ui->setupUi(this);
    ui->tableView->setModel(&m_model);
    connect(app->baseManager, SIGNAL(needUserUpdate()), SLOT(updateData()));
//    connect(m_baseManager, SIGNAL(needUserUpdate()), SLOT(updateGallery()));

    QList<int> carnetKeys = app->baseManager->carnetHash.keys();
    qSort(carnetKeys);

    Q_FOREACH (int key, carnetKeys) {
        CarnetType type = app->baseManager->carnetHash.value(key);
        if (!type.hidden || type.id == CARNET_KEY)
            ui->comboBox->addItem(type.description, type.id);
    }

    updateData();
}

UsersWidget::~UsersWidget()
{
    delete ui;
}

void UsersWidget::on_deleteButton_clicked()
{
    if (!ui->tableView->currentIndex().isValid())
        return;

    quint64 id = m_model.index(ui->tableView->currentIndex().row(), 0).data().value<quint64>();

    QMessageBox *mBox = new QMessageBox;
    mBox->setAttribute(Qt::WA_DeleteOnClose);
    mBox->setWindowTitle(tr("Usuwanie użytkownika"));
    mBox->setText(tr("Czy napewno chcesz usunąć użytkownika?"));
    mBox->setIcon(QMessageBox::Question);
    mBox->setWindowIcon(QIcon("://icons/view-pim-contacts.png"));
    mBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    mBox->setDefaultButton(QMessageBox::Yes);
    int ret = mBox->exec();
    if (ret == QMessageBox::Yes) {

        if (app->baseManager->rmUser(id)) {
            app->baseManager->emitNUU();
        }
    }

}

void UsersWidget::updateData()
{

    QString worker = ui->workerRadio->isChecked() ? " AND type=1" : "";

    QString carnetType = ui->clientRadio->isChecked() ? QString(" AND users.carnettype=%1").arg(ui->comboBox->currentData().toInt()) : "";

    QString excepType = "AND users.id NOT IN (" + QString::number(ONETIMEUSER_NORMAL)
            + ", " + QString::number(ONETIMEUSER_FITNESS_MAN)
            + ", " + QString::number(ONETIMEUSER_FITNESS_WOMAN)
            + ", " + QString::number(ONETIMEUSER_OPEN_MAN)
            + ", " + QString::number(ONETIMEUSER_OPEN_WOMAN) + ")";

    QString stringQuery = QString("SELECT users.id, users.name, users.surname, users.telefon, users.type, users.carnettype FROM users "
                                  "WHERE type!=2 "
                                  "%1 "
                                  "%2 "
                                  "%3 ")
                          .arg(worker)
                          .arg(carnetType)
                          .arg(excepType);

    m_model.setQuery(stringQuery);

     //    qDebug() << Q_FUNC_INFO << worker << carnetType;

    if (m_model.lastError().type() != QSqlError::NoError) {
        app->baseManager->showErrorBox(m_model.lastError().text());
        return;
    }

    m_model.setHeaderData(0, Qt::Horizontal, tr("Id"));
    m_model.setHeaderData(1, Qt::Horizontal, tr("Imię"));
    m_model.setHeaderData(2, Qt::Horizontal, tr("Nazwisko"));
    m_model.setHeaderData(3, Qt::Horizontal, tr("Telefon"));
    m_model.setHeaderData(4, Qt::Horizontal, tr("Typ konta"));
    m_model.setHeaderData(5, Qt::Horizontal, tr("Typ karnetu"));
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    ui->countLabel->setText(QString::number(m_model.rowCount()));
}

void UsersWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        Q_EMIT simulatedEntrance(m_model.index(index.row(), 0).data().value<quint64>());
}

void UsersWidget::on_comboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateData();
}

void UsersWidget::on_workerRadio_toggled(bool checked)
{
    if (checked)
        updateData();
}

void UsersWidget::on_clientRadio_toggled(bool checked)
{
    if (checked)
        updateData();
}
