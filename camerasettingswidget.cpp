#include "camerasettingswidget.h"
#include "ui_camerasettingswidget.h"
#include <QCameraInfo>
#include <QDebug>
#include "application.h"


Q_DECLARE_METATYPE(QCameraInfo)

CameraSettingsWidget::CameraSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSettingsWidget)
{
    ui->setupUi(this);
    init();
}

CameraSettingsWidget::~CameraSettingsWidget()
{
    delete ui;
}

void CameraSettingsWidget::init()
{
    Q_FOREACH(QCameraInfo camInfo, QCameraInfo::availableCameras()) {
        qDebug() << camInfo.description();
        ui->cameraComboBox->addItem(camInfo.description(), camInfo.deviceName());
    }

//    ui->cameraComboBox->setCurrent(m_settings->value("Camera/name", "").toString());

}

void CameraSettingsWidget::saveData()
{
    app->settings->setValue("Camera/name", ui->cameraComboBox->currentData());
}

void CameraSettingsWidget::on_cameraComboBox_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (camera)
        camera->deleteLater();

    if (imageCapture)
        imageCapture->deleteLater();

    camera = new QCamera(ui->cameraComboBox->currentData().toString().toUtf8(), this);
    if (camera) {
        camera->setViewfinder(ui->cameraView);
        imageCapture = new QCameraImageCapture(camera);

        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->start(); // Viewfinder frames start flowing
    }
}

void CameraSettingsWidget::on_buttonBox_clicked(QAbstractButton *aButton)
{
    switch (ui->buttonBox->standardButton(aButton)) {

    case QDialogButtonBox::Apply:
        saveData();
        break;
    case QDialogButtonBox::Cancel:
        close();
        break;
    case QDialogButtonBox::Ok:
        saveData();
        close();
        break;
    default:
        break;
    }
}
