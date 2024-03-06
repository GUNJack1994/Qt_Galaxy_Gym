#include "findwidget.h"
#include "ui_findwidget.h"
#include <QDebug>
#include <QSqlError>
#include "application.h"


FindWidget::FindWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindWidget)
{
    ui->setupUi(this);
    connect(app->baseManager, &BaseManager::readyChanged, this, &FindWidget::onStatusChange);
    connect(app->baseManager, SIGNAL(needUserUpdate()), SLOT(updateData()));
    //m_model = new QSqlQueryModel;
    proxy.setSourceModel(&model);
    QList<int> ignoreList;
    ignoreList << 3;
    proxy.ignoreColumnList = ignoreList;
    ui->tableView->setModel(&proxy);
    ui->tableView->setItemDelegate(&m_delegate);
    ui->tableView->hideColumn(0);
}

FindWidget::~FindWidget()
{
    delete ui;
}

void FindWidget::setDateFilter(FindWidget::ValidOptions option)
{
    m_validFilter = option;

//    updateData();
}

FindWidget::ValidOptions FindWidget::dateFilter()
{
    return m_validFilter;
}

void FindWidget::updateTable()
{
    model.setHeaderData(0, Qt::Horizontal, tr("ID"));
    model.setHeaderData(1, Qt::Horizontal, tr("Nazwisko"));
    model.setHeaderData(2, Qt::Horizontal, tr("Imię"));
    model.setHeaderData(3, Qt::Horizontal, tr("Ważny do"));
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

QString FindWidget::typeFilterString()
{
    QStringList out;
    foreach (int t, typeFilter) {
        out.append(QString::number(t));
    }

    return out.join(", ");
}

void FindWidget::updateData()
{
    qDebug() << Q_FUNC_INFO << "updateData";

    QString carnetTypeString = "";
    if (!typeFilter.isEmpty()) {

        QStringList paramList;
        Q_FOREACH (int param, typeFilter) {
            paramList.append(QString::number(param));
        }

        carnetTypeString = QString(" AND carnettype IN (%1)").arg(paramList.join(", "));
    }

    QString dateString;
    switch (m_validFilter) {
    case ValidActive: {
        dateString = QString(" AND DATE(validto) >= '%1'").arg(QDate::currentDate().addDays(-31).toString("yyyy-MM-dd"));
        break;
    }
    case ValidInactive: {
        dateString = QString(" AND (DATE(validto) < '%1' OR validto IS NULL)").arg(QDate::currentDate().addDays(-31).toString("yyyy-MM-dd"));
        break;
    }
    default:
        break;
    }

    emit downloadUsers(QString("SELECT id, surname, name, validto FROM users WHERE type=0 %1 %2").arg(carnetTypeString).arg(dateString));
}

int FindWidget::invalidCounter()
{
    int counter = 0;
    for (int a = 0; a < proxy.rowCount(); ++a) {
        if (proxy.index(a,3).data().toDate() < QDate::currentDate())
            ++counter;
    }
    return counter;
}

void FindWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        Q_EMIT simulatedEntrance(model.index(proxy.mapToSource(index).row(), 0).data().value<quint64>());
    }
}

void FindWidget::onStatusChange(bool connected)
{
    qDebug() << "status change";
    if (connected && app->baseManager->worker.isValid())
        updateData();
    else {
        model.clear();
        model.removeRows(0, model.rowCount());
    }
}
