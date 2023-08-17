#ifndef XPROTOLABINTERFACE_H
#define XPROTOLABINTERFACE_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include "bootloader.h"

#define MAJOR               0   // Major version
#define MINOR               4   // Minor version

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

        void on_actionAbout_XScopes_Interface_triggered();

    private:
        Ui::XprotolabInterface *ui;
        Bootloader DialogBootloader;
};

#endif // XPROTOLABINTERFACE_H
