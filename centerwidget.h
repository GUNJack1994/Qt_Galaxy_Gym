#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <QWidget>
#include <QQuickView>
#include "userobject.h"
#include "basemanager.h"

namespace Ui {
class CenterWidget;
}

class CenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CenterWidget(QWidget *parent = 0);
    ~CenterWidget();
    void reuse(UserObject &userObj);
    void showKey(UserObject &userObj);

public Q_SLOTS:
    void showUser(const QString &rfid);


private:
    Ui::CenterWidget *ui;
    QQuickView *view;
    BaseManager *m_baseManager;

Q_SIGNALS:
    void needUserWidnow(const QString &rfid);
};

#endif // CENTERWIDGET_H
