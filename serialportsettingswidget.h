#ifndef SERIALPORTSETTINGSWIDGET_H
#define SERIALPORTSETTINGSWIDGET_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class SerialPortSettingsWidget;
}

class SerialPortSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortSettingsWidget(QWidget *parent = 0);
    ~SerialPortSettingsWidget();

private slots:
    void on_buttonBox_clicked(QAbstractButton *aButton);

private:
    Ui::SerialPortSettingsWidget *ui;
    void saveData();
};

#endif // SERIALPORTSETTINGSWIDGET_H
