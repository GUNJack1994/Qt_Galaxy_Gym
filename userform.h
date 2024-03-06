#ifndef USERFORM_H
#define USERFORM_H

#include <QWidget>
#include "userobject.h"
#include <QSqlQueryModel>
#include "paymentdelegate.h"
#include <QStandardItemModel>
#include "entrancedelegate.h"

namespace Ui {
class UserForm;
}

class UserForm : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(WidgetStates currentState READ currentState WRITE setCurrentState NOTIFY currentStateChanged)

public:
    explicit UserForm(QWidget *parent = 0);
    ~UserForm();
    enum WidgetStates {
        EmptyState = 0,
        ValidState,
        InvalidState,
        EditState,
        AddState,
        ExtendedState,
    };

    WidgetStates currentState() const;

private:
    Ui::UserForm *ui;
    QPixmap tempPixmap;
    UserObject m_currentUser;
    void initCombo();
    QSqlQueryModel m_paymentModel;
    QSqlQueryModel m_entranceModel;
    EntranceDelegate m_entranceDelegate;
    PaymentDelegate paymentDelegate;
    bool isActive;
    QStandardItemModel m_rfidModel;
    QDate dateFrom;
    QStandardItemModel m_familyModel;
    UserObject userFromForm();
    WidgetStates m_currentState;
    QString m_tempUuid;

   // void setStateEmpty();
    //void setStateValid();
    //void setStateInvalid();
    //void setStateEdit();
    //void setStateAdd();
   // void setStateExtend();


    void checkRFIDInUse();

private Q_SLOTS:
    void onPixmapToSet(QPixmap, QWidget *);
    void onPhotoDone();
    void onExtend();
    void resetRFIDView();
    void updateEntranceData();
    void updatePaymentData();

public slots:
    void reuse(UserObject &userObj, bool needPaymentUpdate = false, bool fresh = false);
    void reuse(const QString &rfid);
    void onGrabbedRFID(const QString &rfid);
    void setCurrentState(WidgetStates currentState);

private slots:
    void on_okButton_clicked();
    void on_takePhotoButton_clicked();
    void on_extendButton_clicked();
    void on_entranceButton_clicked();
    void on_cancelButton_clicked();
    void on_editButton_clicked();
    void on_addButton_clicked();
    void on_typeCombo_currentIndexChanged(int index);
    void on_addRFIDButton_clicked();
    void on_grabRFIDButton_toggled(bool checked);
    void on_checkRFIDButton_clicked();
    void on_rmRFIDButton_clicked();
    void on_button14d_clicked();
    void on_button1m_clicked();
    void on_button3m_clicked();
    void on_button6m_clicked();
    void on_rmPaymentButton_clicked();
    void on_familyAdd_clicked();
    void on_familiRm_clicked();

    void on_radio14_clicked();

    void on_radio30_clicked();

    void on_addKeyButton_clicked();

Q_SIGNALS:
    void grabRFID();
    void releaseRFID();
    void currentStateChanged(WidgetStates currentState);
};

#endif // USERFORM_H
