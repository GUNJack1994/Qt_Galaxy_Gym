#ifndef DATABASESETTINGSWIDGET_H
#define DATABASESETTINGSWIDGET_H

#include <QWidget>
#include <QAbstractButton>

namespace Ui {
class DataBaseSettingsWidget;
}

class DataBaseSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataBaseSettingsWidget(QWidget *parent = 0);
    ~DataBaseSettingsWidget();

private slots:
    void on_buttonBox_rejected();
    void onButtonBoxClicked(QAbstractButton *button);

private:
    Ui::DataBaseSettingsWidget *ui;
    void updateData();
    void saveData();

Q_SIGNALS:
//    void needReconnect();
};

#endif // DATABASESETTINGSWIDGET_H
