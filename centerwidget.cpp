#include "centerwidget.h"
#include "ui_centerwidget.h"
#include <QGridLayout>
#include <QQuickItem>
#include <QQmlContext>
#include <QBuffer>
#include "application.h"
#include "defines.h"

CenterWidget::CenterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CenterWidget)
{
    ui->setupUi(this);

    m_baseManager = app->baseManager;

    view = new QQuickView();
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    QWidget *container = QWidget::createWindowContainer(view, this);
    view->rootContext()->setContextProperty("bgdColor", palette().background().color());

    view->rootContext()->setContextProperty("centerWidget", this);
    view->setSource(QUrl("qrc:/qml/CenterView.qml"));
    QGridLayout *grid = new QGridLayout;
    setLayout(grid);
    grid->setMargin(0);
    grid->addWidget(container);
}

CenterWidget::~CenterWidget()
{
    delete ui;
}

void CenterWidget::reuse(UserObject &userObj)
{
    QObject *rootObject = view->rootObject();

    Payment currentPayment;
    if (!userObj.paymentList.isEmpty())
        currentPayment = Payment::currentPayment(userObj.paymentList);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    userObj.photo.save(&buffer, "PNG");
    QString iconBase64 = "data:image/png;base64," + QString::fromLatin1(byteArray.toBase64().data());

    QMetaObject::invokeMethod(rootObject, "reuse", Qt::QueuedConnection,
                              Q_ARG(QVariant, userObj.id),
                              Q_ARG(QVariant, userObj.name),
                              Q_ARG(QVariant, userObj.surname),
                              Q_ARG(QVariant, userObj.rfidList.first()),
                              Q_ARG(QVariant, currentPayment.to),
                              Q_ARG(QVariant, currentPayment.ticketTypeDescription),
                              Q_ARG(QVariant, currentPayment.price),
                              Q_ARG(QVariant, iconBase64),
                              Q_ARG(QVariant, currentPayment.isValid()),
                              Q_ARG(QVariant, m_baseManager->carnetHash.value(currentPayment.type).colorString));
}

void CenterWidget::showKey(UserObject &userObj)
{
    QObject *rootObject = view->rootObject();

    QMetaObject::invokeMethod(rootObject, "showKey", Qt::QueuedConnection,
                              Q_ARG(QVariant, userObj.name),
                              Q_ARG(QVariant, userObj.surname),
                              Q_ARG(QVariant, userObj.rfidList.first()));
}

void CenterWidget::showUser(const QString &rfid)
{
    Q_EMIT needUserWidnow(rfid);
}
