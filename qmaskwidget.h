#ifndef QMASKWIDGET_H
#define QMASKWIDGET_H
#include <qtoolbutton.h>
#include <qmap.h>
#include <QFrame>
#include "common_head.h"
#include <qsystemtrayicon.h>

namespace Ui {
class mainframe;
}

class QMaskWidget : public QFrame
{
    Q_OBJECT

public:
    explicit QMaskWidget(QWidget *parent = nullptr);
    ~QMaskWidget();
public:
    void startAnimation();

    void closeWindowAnimation();

    void shakeWindow();

    void shrinkWindow();

    void moveWindow(int duration,int deltx,int delty);

    void initButtons();

    static QVariantList readConfigGroup(const QString& strFile);

    static bool saveConfigGroup(const QString& strFile,QVariantList& listConfig);
protected:
    void timerEvent(QTimerEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);
private slots:
    void on_pbClose_clicked();

    void on_pbConfig_clicked();

    void slotClickToolButton();

    void slotActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon* trayIcon=nullptr; // ²Ëµ¥Í¼±ê
    QMenu* trayMenu = nullptr;

    Ui::mainframe *ui;

    bool m_button_pressed = false;

    bool m_shrinked = false;

    QPoint  m_start_pos;
    QPoint  m_end_pos;
    QMap<QToolButton*,QVariantMap> mapButton;

};

#endif // QMASKWIDGET_H
