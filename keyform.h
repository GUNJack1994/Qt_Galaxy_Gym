#ifndef KEYFORM_H
#define KEYFORM_H

#include <QWidget>

namespace Ui {
class KeyForm;
}

class KeyForm : public QWidget
{
    Q_OBJECT

public:
    explicit KeyForm(const QString &uuid, QWidget *parent = 0);
    ~KeyForm();


private slots:
    void on_pushButton_clicked();

private:
    Ui::KeyForm *ui;
    QString m_uuid;
};

#endif // KEYFORM_H
