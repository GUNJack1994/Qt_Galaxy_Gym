#include "groupedentrancewidget.h"
#include "ui_groupedentrancewidget.h"
#include "application.h"
#include <QDebug>

Q_DECLARE_METATYPE(UserObject)

GroupedEntranceWidget::GroupedEntranceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupedEntranceWidget)
{
    ui->setupUi(this);
    ui->treeView->setModel(&m_modelOut);
    ui->listView->setModel(&m_modelIn);
    ui->listViewIn->setModel(&m_modelSelected);

    m_lastId = -1;
    m_lastDateTime = QDateTime();
    ui->fromDateEdit->setDate(QDate::currentDate());
    ui->toDateEdit->setDate(QDate::currentDate());

    updateAllUserModel();
}

GroupedEntranceWidget::~GroupedEntranceWidget()
{
    delete ui;
}

void GroupedEntranceWidget::on_okButton_clicked()
{
    QList<UserObject> userList = app->baseManager->groupedEntrance(ui->fromDateEdit->date(), ui->toDateEdit->date());

    m_modelOut.clear();
    Q_FOREACH (UserObject user, userList) {

        if (user.entrance.date() != m_lastDateTime.date()) {
            m_lastId = -1;
        }

        if (idList.contains(user.id)) {
            m_lastId = user.id;
            m_lastDateTime = user.entrance;

            m_modelOut.appendRow(new QStandardItem(QString("%1 %2 - %3")
                                                   .arg(user.surname)
                                                   .arg(user.name)
                                                   .arg(user.entrance.toString("yyyy-MM-dd hh:mm:ss"))));
            m_lastIndex = m_modelOut.index(m_modelOut.rowCount() -1, 0);
            continue;
        }

        if (m_lastId > 0) {
            m_modelOut.itemFromIndex(m_lastIndex)->appendRow(new QStandardItem(QString("%1 %2 - %3")
                                                   .arg(user.surname)
                                                   .arg(user.name)
                                                   .arg(user.entrance.toString("yyyy-MM-dd hh:mm:ss"))));
        }
    }
}

void GroupedEntranceWidget::updateAllUserModel()
{
    QList<UserObject> userList = app->baseManager->allUserList();

    m_modelIn.clear();
    Q_FOREACH (UserObject user, userList) {
        QStandardItem *item = new QStandardItem(QString("%1 %2").arg(user.surname).arg(user.name));

        item->setData(QVariant::fromValue<UserObject>(user));

        m_modelIn.appendRow(item);
    }
}

void GroupedEntranceWidget::on_addButton_clicked()
{
    QModelIndex index = ui->listView->currentIndex();
    if (!index.isValid())
        return;

    int id = index.data(Qt::UserRole + 1).value<UserObject>().id;
    if (!idList.contains(id)) {
        idList << id;
        QStandardItem *item = new QStandardItem(QString("%1 %2").arg(index.data(Qt::UserRole + 1).value<UserObject>().surname).arg(index.data(Qt::UserRole + 1).value<UserObject>().name));
        item->setData(QVariant::fromValue<UserObject>(index.data(Qt::UserRole + 1).value<UserObject>()));
        m_modelSelected.appendRow(item);
    }
}

void GroupedEntranceWidget::on_rmButton_clicked()
{
    QModelIndex index = ui->listViewIn->currentIndex();
    if (!index.isValid())
        return;

    int id = index.data(Qt::UserRole + 1).value<UserObject>().id;
    if (idList.contains(id)) {
        idList.removeAll(id);
    }

    m_modelSelected.removeRow(index.row());
}
