#include "takephotowidget.h"
#include "ui_takephotowidget.h"
#include <QDebug>
#include <QFile>
#include <QScrollBar>
#include "application.h"

TakePhotoWidget::TakePhotoWidget(QPixmap &tempPixmap, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TakePhotoWidget),
    m_tempPixmap(tempPixmap)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&scene);
    scene.addItem(&pixmapItem);

    QString cameraName = app->settings->value("Camera/name").toString();

    if (cameraName.isEmpty()) {
        qWarning() << "brak ustawionej kamery";
        this->setEnabled(false);
        return;
    }

    camera = new QCamera(app->settings->value("Camera/name").toString().toUtf8(), this);
    if (camera) {
        camera->setViewfinder(ui->cameraView);
        imageCapture = new QCameraImageCapture(camera, this);

        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->start(); // Viewfinder frames start flowing

        connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
        connect(imageCapture, SIGNAL(imageSaved(int,QString)), SLOT(onImageSaved(int,QString)));
    }
}

TakePhotoWidget::~TakePhotoWidget()
{
    delete ui;
}

void TakePhotoWidget::on_takePhotoButton_clicked()
{
    imageCapture->capture(qApp->applicationDirPath() + "/rfidTempFile");
}

void TakePhotoWidget::processCapturedImage(int id, QImage image)
{
    Q_UNUSED(id);
    capturedImage = image;
    pixmapItem.setPixmap(QPixmap::fromImage(capturedImage));
}

void TakePhotoWidget::onImageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    QFile tempImgFile(fileName);
    if(!tempImgFile.remove())
        qWarning() << "can't delete temp image" << tempImgFile.errorString();
}

void TakePhotoWidget::on_okButton_clicked()
{
    int x = ui->graphicsView->horizontalScrollBar()->value();
    int y = ui->graphicsView->verticalScrollBar()->value();
    qreal scale = pixmapItem.scale();
    int newX = x / scale;
    int newY = y / scale;
    int newWidth = 200 / scale;

    m_tempPixmap = QPixmap::fromImage(capturedImage.copy(newX, newY, newWidth, newWidth));

    Q_EMIT photoDone();
    close();
}

void TakePhotoWidget::on_cancelButton_clicked()
{
    close();
}

void TakePhotoWidget::on_verticalSlider_valueChanged(int value)
{
    Q_UNUSED(value);
    pixmapItem.setScale( (qreal)(ui->verticalSlider->value() + 100) / 100);
}

void TakePhotoWidget::on_plusButton_clicked()
{
    ui->verticalSlider->setValue(ui->verticalSlider->value() + 1);
}

void TakePhotoWidget::on_minusButton_clicked()
{
    ui->verticalSlider->setValue(ui->verticalSlider->value() - 1);
}
