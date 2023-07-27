#ifndef XPROTOLABINTERFACE_H
#define XPROTOLABINTERFACE_H

#include <QMainWindow>
#include "bootloader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class XprotolabInterface; }
QT_END_NAMESPACE

class XprotolabInterface : public QMainWindow {
    Q_OBJECT
    public:
        XprotolabInterface(QWidget *parent = nullptr);
        ~XprotolabInterface();
    private slots:
        void on_actionUpgrade_Firmware_triggered();

    private:
        Ui::XprotolabInterface *ui;
        Bootloader DialogBootloader;
};

#endif // XPROTOLABINTERFACE_H
