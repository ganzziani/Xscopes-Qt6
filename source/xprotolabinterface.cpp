#include "xprotolabinterface.h"
#include "./ui_xprotolabinterface.h"

XprotolabInterface::XprotolabInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::XprotolabInterface) {
    ui->setupUi(this);
}

XprotolabInterface::~XprotolabInterface() {
    delete ui;
}

void XprotolabInterface::on_actionUpgrade_Firmware_triggered() {
    DialogBootloader.setWindowModality(Qt::ApplicationModal);
    DialogBootloader.show();
}
