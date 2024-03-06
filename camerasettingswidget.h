#ifndef CAMERASETTINGSWIDGET_H
#define CAMERASETTINGSWIDGET_H

#include <QWidget>
#include <QCamera>
#include <QPointer>
#include <QCameraImageCapture>
#include <QAbstractButton>

namespace Ui {
class CameraSettingsWidget;
}

class CameraSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraSettingsWidget(QWidget *parent = 0);
    ~CameraSettingsWidget();

private slots:
    void on_cameraComboBox_currentIndexChanged(const QString &arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::CameraSettingsWidget *ui;
    void init();
    QPointer<QCamera> camera;
    QPointer<QCameraImageCapture> imageCapture;
    void saveData();
};

#endif // CAMERASETTINGSWIDGET_H
