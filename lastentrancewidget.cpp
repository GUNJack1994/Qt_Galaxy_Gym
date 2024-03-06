#include "lastentrancewidget.h"
#include "ui_lastentrancewidget.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "application.h"

LastEntranceWidget::LastEntranceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LastEntranceWidget),
    m_currentReader(0)
{
    ui->setupUi(this);

    connect(app->baseManager, &BaseManager::needEntranceUpdate, this, &LastEntranceWidget::updateData);
    ui->tableView->setModel(&m_model);
    ui->tableView->setItemDelegate(&delegate);
}

LastEntranceWidget::~LastEntranceWidget()
{
    delete ui;
}

int LastEntranceWidget::currentReader() const
{
    return m_currentReader;
}

void LastEntranceWidget::updateData()
{
    m_model.setQuery(QString("SELECT entrance.user, entrance.valid, users.surname, users.name, entrance.date, entrance.reader FROM entrance INNER JOIN users ON entrance.user=users.id"
                             " WHERE entrance.date>=%1 AND entrance.reader=%2 ORDER BY entrance.date DESC").arg(QDate::currentDate().toString("\"yyyy-MM-dd\"")).arg(m_currentReader));


    if (m_model.lastError().type() != QSqlError::NoError) {
        app->baseManager->showErrorBox(m_model.lastError().text());
        return;
    }

    m_model.setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_model.setHeaderData(2, Qt::Horizontal, tr("Nazwisko"));
    m_model.setHeaderData(3, Qt::Horizontal, tr("Imię"));
    m_model.setHeaderData(4, Qt::Horizontal, tr("Godzina"));
    ui->tableView->hideColumn(1);
    ui->tableView->hideColumn(5);
    if (m_model.rowCount() > 0) {
        ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    }

//    ui->entranceCounter->display(m_model.rowCount());
    ui->entranceCounterLabel->setText(QString::number(m_model.rowCount()));
}

void LastEntranceWidget::setCurrentReader(int currentReader)
{
    if (m_currentReader == currentReader)
        return;

    m_currentReader = currentReader;
    emit currentReaderChanged(currentReader);
}

void LastEntranceWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        Q_EMIT simulatedEntrance(m_model.index(index.row(), 0).data().value<quint64>());
    }
}

void LastEntranceWidget::on_reader0button_clicked()
{
    setCurrentReader(0);
    updateData();
}

void LastEntranceWidget::on_reader1button_clicked()
{
    setCurrentReader(1);
    updateData();
}

void LastEntranceWidget::on_reader2button_clicked()
{
    setCurrentReader(2);
    updateData();
}
