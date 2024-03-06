#ifndef GROUPEDENTRANCEWIDGET_H
#define GROUPEDENTRANCEWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDateTime>

namespace Ui {
class GroupedEntranceWidget;
}

class GroupedEntranceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupedEntranceWidget(QWidget *parent = 0);
    ~GroupedEntranceWidget();

private slots:
    void on_okButton_clicked();
    void updateAllUserModel();

    void on_addButton_clicked();

    void on_rmButton_clicked();

private:
    Ui::GroupedEntranceWidget *ui;
    QStandardItemModel m_modelOut;
    QStandardItemModel m_modelIn;
    QStandardItemModel m_modelSelected;
    QList<int> idList;
    QDateTime m_lastDateTime;
    int m_lastId;
    QModelIndex m_lastIndex;
};

#endif // GROUPEDENTRANCEWIDGET_H
