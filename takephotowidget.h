#ifndef TAKEPHOTOWIDGET_H
#define TAKEPHOTOWIDGET_H

#include <QWidget>
#include <QCamera>
#include <QPointer>
#include <QCameraImageCapture>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace Ui {
class TakePhotoWidget;
}

class TakePhotoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TakePhotoWidget(QPixmap &tempPixamp, QWidget *parent = 0);
    ~TakePhotoWidget();

private slots:
    void on_takePhotoButton_clicked();
    void processCapturedImage(int,QImage);
    void onImageSaved(int, const QString &);
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_plusButton_clicked();

    void on_minusButton_clicked();

private:
    Ui::TakePhotoWidget *ui;
    QPointer<QCamera> camera;
    QPointer<QCameraImageCapture> imageCapture;
    QPixmap &m_tempPixmap;
    QImage capturedImage;
    QGraphicsScene scene;
    QGraphicsPixmapItem pixmapItem;

Q_SIGNALS:
    void pixmapToSet(QPixmap, QWidget*);
    void photoDone();
};

#endif // TAKEPHOTOWIDGET_H
