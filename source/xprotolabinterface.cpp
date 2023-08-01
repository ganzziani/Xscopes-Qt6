#include "xprotolabinterface.h"
#include "./ui_xprotolabinterface.h"
#include "build.h"  // Contains build number (increases automatically with batch file)

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

void XprotolabInterface::on_actionAbout_XScopes_Interface_triggered() {
    QDialog msgBox;
    QVBoxLayout VLayout;
    QLabel logo,text;
    logo.setPixmap(QPixmap(":/images/images/gabotronics-hit.png"));
    text.setText(QString("<p><b>XScope Interface Software</b><br>Version %1.%2 ").arg(MAJOR).arg(MINOR, 2, 10, QChar('0')) +
                 QString("Build %1<br>Compilation date: ").arg(BUILD) + QString(__DATE__) +
                 "<br><a href=\"https://gabotronics.com/\">Gabotronics</a>");
    text.setOpenExternalLinks(true);
    VLayout.addWidget(&logo);
    VLayout.addWidget(&text);
    VLayout.setSpacing(10);
    msgBox.setLayout(&VLayout);
    msgBox.setWindowIcon(QIcon(QPixmap(":/images/images/GT-icon16.png")));
    msgBox.setWindowTitle(tr("About XScopes Interface"));
    msgBox.layout()->setSizeConstraint(QLayout::SetFixedSize);
    msgBox.exec();
}
