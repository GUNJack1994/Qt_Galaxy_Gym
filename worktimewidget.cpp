#include "worktimewidget.h"
#include "ui_worktimewidget.h"
#include <QDebug>
#include <QSqlError>
#include "application.h"

WorktimeWidget::WorktimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorktimeWidget)
{
    qDebug() << Q_FUNC_INFO;
    ui->setupUi(this);

    connect(app->baseManager, SIGNAL(needUserUpdate()), SLOT(updateData()));
    ui->tableView->setModel(&m_model);
}

WorktimeWidget::~WorktimeWidget()
{
    delete ui;
}

void WorktimeWidget::updateData()
{
    m_model.setQuery("SELECT worktime.date, users.surname, users.name FROM worktime "
                     "INNER JOIN users ON users.id=worktime.user ORDER BY worktime.date DESC");

    if (m_model.lastError().type() != QSqlError::NoError) {
        app->baseManager->showErrorBox(m_model.lastError().text());
        return;
    }

    m_model.setHeaderData(0, Qt::Horizontal, tr("Zalogowano"));
    m_model.setHeaderData(1, Qt::Horizontal, tr("Nazwisko"));
    m_model.setHeaderData(2, Qt::Horizontal, tr("Imię"));
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}
