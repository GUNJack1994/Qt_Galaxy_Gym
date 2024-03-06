#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPointer>
#include <QTimer>
#include "userform.h"
#include "userform.h"
#include <QStateMachine>
#include "simulatorrfid.h"
#include "userswidget.h"
#include "statuscontrolswidget.h"
#include "photodelegator.h"
#include <QLabel>
#include <QGraphicsColorizeEffect>
#include "querythread.h"
#include <QThread>
#include "findwidget.h"
#include "rowfilter.h"
#include "socketserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void onCard(const QString &id);

private slots:
    void showDataBaseSettingsWidget();
    void showSerialPortSettingsWidget();
    void showCameraSettingsWidget();
    void showFindUserWidget();
    void showSalesWidget();
    void showWorktimeWidget();
    void on_loginButton_clicked();
    void on_passLineEdit_returnPressed();
    void showUserForm(UserObject &);
    void showUsersWidget();
    void logIn(UserObject &user);
    void logOff();
    void grabRFID();
    void releaseRFID();

    void on_galleryButton_toggled(bool checked);

    void updatePhotoModel();
//    void updateUserModel();

    void on_photoView_2_entered(const QModelIndex &index);

    void checkSplash();

    void onPhotoListReady(PixHash);

    void onUserListReady(VariantHash);

    void on_photoView_2_doubleClicked(const QModelIndex &index);

    void on_oneTimeButton_clicked();

    void on_oneTimeButton1_clicked();

    void on_oneTimeButton2_clicked();

    void tryAbortGalleryPopulation();

    void on_invalidButton_clicked();

    void on_textFilterButton_clicked();

    void on_normalButton_clicked(bool checked);

    void on_halfButton_clicked(bool checked);

    void on_ladiesButton_clicked(bool checked);

    void on_allButton_clicked();

    void on_activeButton_clicked();

    void on_textFilterEdit_returnPressed();

    void on_clearTextFilterButton_clicked();

    void on_doorButton_clicked();

    //void on_openWomanButton_clicked();

    void checkEntryButtons();

    void on_actionWej_cia_grupowane_triggered();

public Q_SLOTS:
    void showUserForm(const QString &rfid);
    void showUserForm(quint64 id);

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent*);
    bool logInWorkerByRfid(const QString &rfid);
    SimulatorRFID *simulator;
    QPointer<UsersWidget> usersWidget;
    StatusControlsWidget controlsWidget;
    bool needGrabRFID;
    QStandardItemModel m_photoModel;
    PhotoDelegator m_photoDelegator;
    QPointer<QLabel> m_photoSplash;
    QTimer m_splashTimer;
    QueryThread *m_queryThread;
    QThread m_thread;
    void updateCounters();
    void setFilter(bool checked, QList<int> carnetType);
    void setValidFilter(FindWidget::ValidOptions option);
    RowFilter m_photoModelProxy;
    void waitBox();
    SocketServer m_socketServer;

    void oneTimeEntry(int user, int type, bool banan = false);


Q_SIGNALS:
    void grabbedRFID(const QString &rfid);
    void downloadPhotos(const QString &params = "");
};

#endif // MAINWINDOW_H
