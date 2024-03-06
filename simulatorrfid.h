#ifndef SIMULATORRFID_H
#define SIMULATORRFID_H

#include <QWidget>

namespace Ui {
class SimulatorRFID;
}

class SimulatorRFID : public QWidget
{
    Q_OBJECT

public:
    explicit SimulatorRFID(QWidget *parent = 0);
    ~SimulatorRFID();

private slots:
    void on_pushButton_clicked();

Q_SIGNALS:
    void simRFID(const QString &);

private:
    Ui::SimulatorRFID *ui;
};

#endif // SIMULATORRFID_H
