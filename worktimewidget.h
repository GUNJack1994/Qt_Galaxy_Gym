#ifndef WORKTIMEWIDGET_H
#define WORKTIMEWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>

namespace Ui {
class WorktimeWidget;
}

class WorktimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorktimeWidget(QWidget *parent = 0);
    ~WorktimeWidget();

private:
    Ui::WorktimeWidget *ui;
    QSqlQueryModel m_model;

public Q_SLOTS:
    void updateData();
};

#endif // WORKTIMEWIDGET_H
