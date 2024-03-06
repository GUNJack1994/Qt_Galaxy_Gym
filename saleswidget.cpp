#include "saleswidget.h"
#include "ui_saleswidget.h"
#include <QDebug>
#include <QFileDialog>

SalesWidget::SalesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SalesWidget)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->tableView->setModel(&m_salesModel);
    updateData();
}

SalesWidget::~SalesWidget()
{
    delete ui;
}

void SalesWidget::updateData()
{
    QString dayString;

    if (ui->dayRadio->isChecked()) {
        dayString = "DATE(payment.date)='" + ui->dateEdit->date().toString("yyyy-MM-dd") + "'";
    } else {
        dayString = "YEAR(payment.date)="+ QString::number(ui->dateEdit->date().year()) + " AND MONTH(payment.date)="+ QString::number(ui->dateEdit->date().month());
    }

    m_salesModel.setQuery(QString("SELECT "
                                  "payment.date, payment.price, payUser.id, payUser.name, payUser.surname, types.description, payWorker.name, payWorker.surname"
                                  " FROM payment "
                                  "INNER JOIN users AS payUser on payUser.id=payment.user "
                                  "INNER JOIN types on payment.type=types.id "
                                  "INNER JOIN users AS payWorker on payWorker.id=payment.worker "
                                  "WHERE %1").arg(dayString));

    m_salesModel.setHeaderData(0, Qt::Horizontal, tr("Data"), Qt::DisplayRole);
    m_salesModel.setHeaderData(1, Qt::Horizontal, tr("Cena"), Qt::DisplayRole);
    m_salesModel.setHeaderData(2, Qt::Horizontal, tr("ID użytkownika"), Qt::DisplayRole);
    m_salesModel.setHeaderData(3, Qt::Horizontal, tr("Imię"), Qt::DisplayRole);
    m_salesModel.setHeaderData(4, Qt::Horizontal, tr("Nazwisko"), Qt::DisplayRole);
    m_salesModel.setHeaderData(5, Qt::Horizontal, tr("Opis"), Qt::DisplayRole);
    m_salesModel.setHeaderData(6, Qt::Horizontal, tr("Imię (sprzed.)"), Qt::DisplayRole);
    m_salesModel.setHeaderData(7, Qt::Horizontal, tr("Nazwisko (sprzed.)"), Qt::DisplayRole);

    if (ui->tableView->horizontalHeader()->count() > 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    qreal price = 0;

    for (int a = 0; a < m_salesModel.rowCount(); ++a) {
        price += m_salesModel.index(a,1).data().toReal();
    }

    QLocale locale;
    ui->priceLabel->setText(tr("Kwota: ") + QString::number(price) + " " + locale.currencySymbol());
}

void SalesWidget::on_dateEdit_dateChanged(const QDate &date)
{
    updateData();
}

void SalesWidget::on_dayRadio_toggled(bool checked)
{
    if (checked)
        updateData();
}

void SalesWidget::on_monthRadio_toggled(bool checked)
{
    if (checked)
        updateData();
}

void SalesWidget::on_pushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Eksportuj do csv."),QString(),"*.csv");

    if (filename.isEmpty())
        return;

    QString outString;

    QStringList titles;
    for (int a = 0; a < ui->tableView->horizontalHeader()->count(); ++a) {
        titles << m_salesModel.headerData(a, Qt::Horizontal).toString();
    }

    outString.append(titles.join(";"));
    outString.append("\n");

    for (int a = 0; a < m_salesModel.rowCount(); ++a) {
        QStringList rowStringList;
        for (int b = 0; b < m_salesModel.columnCount(); ++b) {
            rowStringList.append(m_salesModel.index(a,b).data().toString());
        }
        outString.append(rowStringList.join(";"));
        outString.append("\n");
    }

    QFile file(filename + ".csv");
    if (file.open(QFile::WriteOnly)) {
        file.write(outString.toUtf8());
        file.close();
    }
}

