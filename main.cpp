#include "consolewidget.h"
#include <QApplication>
#include "common_head.h"
#include "qmaskwidget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   if(!AppManager::getInstance()->regApp("console"))
    {
        QMessageBox::warning(nullptr,CHINESE_CONVERT("告警"),CHINESE_CONVERT("注册失败"));
        return -1;
    }
    QMaskWidget w;
    w.show();


    return a.exec();
}
