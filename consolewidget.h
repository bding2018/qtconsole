#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QMainWindow>
#include "setdialog.h"
#include "mybutton.h"
#include <QGridLayout>

namespace Ui {
class consoleWidget;
}

class consoleWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit consoleWidget(QWidget *parent = nullptr);
    ~consoleWidget();



private slots:
    void on_btnSet_clicked();
    void slotAppBtnClicked(const QString &appPath,const QString &para);
    void initAppBtn();

private:
    Ui::consoleWidget *ui;



};

#endif // CONSOLEWIDGET_H
