#include "bootloader.h"
#include "ui_bootloader.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>
#include <QFile>
#include <QSettings>

Bootloader::Bootloader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bootloader) {
    ui->setupUi(this);
    ui->textEdit->clear();
    if(ui->radioOW->isChecked())
        on_radioOW_clicked();
    myProcess = new QProcess(this);
    program = "dfu-programmer";
    doEverything = false;
    HEXDownloader = new FileDownloader(0, HEXFileUrl, this);
    EEPDownloader = new FileDownloader(0, EEPFileUrl, this);
    connect(HEXDownloader, SIGNAL(downloaded(QNetworkReply::NetworkError, QString)), this, SLOT(HEXDownloaded(QNetworkReply::NetworkError, QString)));
    connect(EEPDownloader, SIGNAL(downloaded(QNetworkReply::NetworkError, QString)), this, SLOT(EEPDownloaded(QNetworkReply::NetworkError, QString)));
    connect(myProcess, SIGNAL(finished(int)), this, SLOT(ProcessDone(int)));
    connect(myProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(ShowProcessError(QProcess::ProcessError)));
    // Organization name used for QSettings:
    QCoreApplication::setOrganizationName("Gabotronics");
    QCoreApplication::setOrganizationDomain("gabotronics.com");
    QCoreApplication::setApplicationName("XScopes Interface");
    QSettings settings;
    workingDir = settings.value("workingDir").toString();
    ui->lineFlash->setText(settings.value("lastHEXFile").toString());
    ui->lineEEPROM->setText(settings.value("lastEEPFile").toString());
    if(!QDir("firmware").exists()) {    // Create firmware folder if it doesn't exist
        QDir().mkdir("firmware");
    }
    if(QSslSocket::supportsSsl()) {
        ui->textEdit->append("SslSupport ok: ");
        ui->textEdit->append("SslLibraryBuildVersion: " + QSslSocket::sslLibraryBuildVersionString());
        ui->textEdit->append("SslLibraryRuntimeVersion: " + QSslSocket::sslLibraryVersionString());
    }
    else ui->textEdit->append("Error: No SslSupport!");
}

Bootloader::~Bootloader() {
    QSettings settings;
    settings.setValue("workingDir", workingDir);
    settings.setValue("lastHEXFile", ui->lineFlash->text());
    settings.setValue("lastEEPFile", ui->lineEEPROM->text());
    delete ui;
}

QString Bootloader::SelectFile(bool LoadHEX) {
    QFileDialog dialog;
    QString newFileName;
    if(QDir(workingDir).exists())
        dialog.setDirectory(workingDir);
    dialog.setLabelText(QFileDialog::Accept, "Select");
    if(LoadHEX) {
        dialog.setDefaultSuffix("hex");
        dialog.setNameFilter(tr("HEX Files (*.hex);;All Files (*.*)"));
        dialog.setWindowTitle("Select HEX File");
    } else {
        dialog.setDefaultSuffix("eep");
        dialog.setNameFilter(tr("EEPROM Files (*.eep);;All Files (*.*)"));
        dialog.setWindowTitle("Select EEPROM File");
    }
    //dialog.setWindowIcon(logo_icon);
    //dialog.setOption(QFileDialog::DontUseNativeDialog);
    dialog.exec();
    if(dialog.selectedFiles().size()) {
        newFileName = dialog.selectedFiles()[0];
        if(!newFileName.isEmpty()) {
            QFileInfo fileInfo(newFileName);
            workingDir = fileInfo.absolutePath();
        }
    }
    return newFileName;
}

void Bootloader::on_pushSelectFlashFile_clicked() {
    ui->lineFlash->setText(SelectFile(true));
}

void Bootloader::on_pushSelectEEPROMFile_clicked() {
    ui->lineEEPROM->setText(SelectFile(false));
}

void Bootloader::on_pushReadFlash_clicked() {
    ui->textEdit->append("Reading Flash...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "read";
    myProcess->start(program, arguments);
}

void Bootloader::on_pushReadEE_clicked() {
    ui->textEdit->append("Reading EEPROM...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "read" << "--eeprom";
    myProcess->start(program, arguments);
}

void Bootloader::on_pushErase_clicked() {
    ui->textEdit->append("Erasing memory...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "erase";
    myProcess->start(program, arguments);
}

void Bootloader::on_pushFlash_clicked() {
    ui->textEdit->append("Programming Flash...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "flash" << ui->lineFlash->text() << "--ignore-outside";
    myProcess->start(program, arguments);
}

void Bootloader::on_pushEEPROM_clicked() {
    ui->textEdit->append("Programming EEPROM...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "flash" << "--eeprom" << ui->lineEEPROM->text() << "--force";
    myProcess->start(program, arguments);
}

void Bootloader::on_pushStart_clicked() {
    ui->textEdit->append("Starting Application...");
    arguments.clear();
    arguments << ui->lineTarget->text() << "start";
    myProcess->start(program, arguments);
    doEverything = false;
}

void Bootloader::on_pushDoEverything_clicked() {
    if(ui->lineFlash->text().isEmpty()) {
        ui->textEdit->append("No Flash file selected");
        return;
    }
    if(!ui->checkPreserveEE->isChecked() && ui->lineEEPROM->text().isEmpty()) {
        ui->textEdit->append("No EEPROM file selected");
        return;
    }
    ui->textEdit->append("Performing Firmware Update...");
    doEverything = true;
    if(ui->checkPreserveEE->isChecked())
        on_pushReadEE_clicked();
    else
        on_pushErase_clicked();
}

void Bootloader::HEXDownloaded(QNetworkReply::NetworkError error, QString ErrorString) {
    if(error) {
        ui->textEdit->append(QString("Could not download HEX File, error: " + ErrorString));
        ShowDownloadError(error);
    } else {
        QString fileName = HEXDownloader->CurrentUrl().fileName();
        fileName.prepend("firmware/");
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream stream(&file);
        stream << HEXDownloader->downloadedData();
        file.close();
        ui->lineFlash->setText(fileName);
        ui->textEdit->append("HEX File Downloaded successfully");
    }
}

void Bootloader::EEPDownloaded(QNetworkReply::NetworkError error, QString ErrorString) {
    if(error) {
        ui->textEdit->append(QString("Could not download EEP File, error: " + ErrorString));
        ShowDownloadError(error);
    } else {
        QString fileName = EEPDownloader->CurrentUrl().fileName();
        fileName.prepend("firmware/");
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream stream(&file);
        stream << EEPDownloader->downloadedData();
        file.close();
        ui->lineEEPROM->setText(fileName);
        ui->textEdit->append("EEP File Downloaded successfully");
    }
}

void Bootloader::on_pushAutoLoad_clicked() {
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXDownloader->newUrl(HEXFileUrl);  // Download it!
    EEPDownloader->newUrl(EEPFileUrl);  // Download it!
}

void Bootloader::on_radioOW_clicked() {
    ui->lineTarget->setText("atxmega256a3u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/oscilloscope-watch.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/oscilloscope-watch.eep");
}

void Bootloader::on_radioXprotolab_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab.eep");
}

void Bootloader::on_radioXminilab_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab.eep");
}

void Bootloader::on_radioXminilabB_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab2_4.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab2_4.eep");
}

void Bootloader::on_radioXprotolabPortable_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab-portable.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab-portable.eep");
}

void Bootloader::on_radioXminiulabPortable_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab-portable1_1.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab-portable1_1.hex");
}

void Bootloader::on_radioXminiulabPortable1_11_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab-portable1_11.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xminilab-portable1_11.eep");
}

void Bootloader::on_radioXprotolabPlain_clicked() {
    ui->lineTarget->setText("atxmega32a4u");
    ui->lineEEPROM->clear();
    ui->lineFlash->clear();
    HEXFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab-plain.hex");
    EEPFileUrl = QUrl("http://www.gabotronics.com/download/firmware/xprotolab-plain.eep");
}

void Bootloader::on_checkPreserveEE_toggled(bool checked) {
    if(checked) {
        ui->pushDoEverything->setText("Read EEPROM -> Erase Chip -> Program Flash -> Program EEPROM -> Start Application");
        ui->pushSelectEEPROMFile->setEnabled(false);
        ui->lineEEPROM->setEnabled(false);
    } else {
        ui->pushDoEverything->setText("Erase Chip -> Program Flash -> Program EEPROM -> Start Application");
        ui->pushSelectEEPROMFile->setEnabled(true);
        ui->lineEEPROM->setEnabled(true);
    }
}

// The dfu process has completed an operation
void Bootloader::ProcessDone(int exit_val) {
    ui->textEdit->append(myProcess->readAllStandardError());
    if(arguments.contains("read")) {
        QByteArray ProcessOutput = myProcess->readAllStandardOutput();
        QFile file;
        if(arguments.contains("--eeprom"))
            file.setFileName(QString("firmware/") + ui->lineTarget->text() + ".eep");
        else
            file.setFileName(QString("firmware/") + ui->lineTarget->text() + ".hex");
        if(!file.open(QFile::WriteOnly | QFile::Text)) {
            ui->textEdit->append("Can't open file: " + file.fileName());
            return;
        }
        if(arguments.contains("--eeprom"))
            ui->lineEEPROM->setText(file.fileName());
        else ui->lineFlash->setText(file.fileName());
        if(ProcessOutput.length()) {
            file.write(ProcessOutput.data(), ProcessOutput.length());
            file.close();
            ui->textEdit->append("File saved at: " + file.fileName());
        }
    } else {
        ui->textEdit->append(myProcess->readAllStandardOutput());
    }
    if(doEverything) {  // Doing all operations?
        if(arguments.contains("read") && arguments.contains("--eeprom"))
            on_pushErase_clicked();
        else if(arguments.contains("erase"))
            on_pushFlash_clicked();
        else if(arguments.contains("flash") && !arguments.contains("--eeprom"))
            on_pushEEPROM_clicked();
        else if(arguments.contains("flash") &&  arguments.contains("--eeprom"))
            on_pushStart_clicked();
        else
            doEverything = false;
    }
}

void Bootloader::ShowProcessError(QProcess::ProcessError error) {
    ui->textEdit->append("Error when starting dfu-programmer.exe process:");
    switch(error) {
        case QProcess::FailedToStart:
            ui->textEdit->append("The process failed to start. Either the invoked program is missing, or you may have insufficient permissions or resources to invoke the program.");
        break;
        case QProcess::Crashed:
            ui->textEdit->append("The process crashed some time after starting successfully.");
        break;
        case QProcess::Timedout:
            ui->textEdit->append("The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.");
        break;
        case QProcess::WriteError:
            ui->textEdit->append("An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.");
        break;
        case QProcess::ReadError:
            ui->textEdit->append("An error occurred when attempting to read from the process. For example, the process may not be running.");
        break;
        case QProcess::UnknownError:
            ui->textEdit->append("An unknown error occurred.");
        break;

    }
}

void Bootloader::ShowDownloadError(QNetworkReply::NetworkError error) {
    QString text = QString("Error %1: ").arg(error);
    switch(error) {
        case QNetworkReply::NoError:
            text += "No error";
        break;
        case QNetworkReply::ConnectionRefusedError:
            text += "The remote server refused the connection (the server is not accepting requests)";
        break;
        case QNetworkReply::RemoteHostClosedError:
            text += "The remote server closed the connection prematurely, before the entire reply was received and processed";
        break;
        case QNetworkReply::HostNotFoundError:
            text += "The remote host name was not found (invalid hostname)";
        break;
        case QNetworkReply::TimeoutError:
            text += "The connection to the remote server timed out";
        break;
        case QNetworkReply::OperationCanceledError:
            text += "The operation was canceled via calls to abort() or close() before it was finished.";
        break;
        case QNetworkReply::SslHandshakeFailedError:
            text += "The SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted.";
        break;
        case QNetworkReply::TemporaryNetworkFailureError:
            text += "The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established.";
        break;
        case QNetworkReply::NetworkSessionFailedError:
        text += "The connection was broken due to disconnection from the network or failure to start the network.";
        break;
        case QNetworkReply::BackgroundRequestNotAllowedError:
            text += "The background request is not currently allowed due to platform policy.";
        break;
        case QNetworkReply::TooManyRedirectsError:
            text += "While following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)";
        break;
        case QNetworkReply::InsecureRedirectError:
            text += "While following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)";
        break;
        case QNetworkReply::ProxyConnectionRefusedError:
            text += "The connection to the proxy server was refused (the proxy server is not accepting requests)";
        break;
        case QNetworkReply::ProxyConnectionClosedError:
            text += "The proxy server closed the connection prematurely, before the entire reply was received and processed";
        break;
        case QNetworkReply::ProxyNotFoundError:
            text += "The proxy host name was not found (invalid proxy hostname)";
        break;
        case QNetworkReply::ProxyTimeoutError:
            text += "The connection to the proxy timed out or the proxy did not reply in time to the request sent";
        break;
        case QNetworkReply::ProxyAuthenticationRequiredError:
            text += "The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)";
        break;
        case QNetworkReply::ContentAccessDenied:
            text += "The access to the remote content was denied (similar to HTTP error 403)";
        break;
        case QNetworkReply::ContentOperationNotPermittedError:
            text += "The operation requested on the remote content is not permitted";
        break;
        case QNetworkReply::ContentNotFoundError:
            text += "The remote content was not found at the server (similar to HTTP error 404)";
        break;
        case QNetworkReply::AuthenticationRequiredError:
            text += "The remote server requires authentication to serve the content but the credentials provided were not accepted (if any)";
        break;
        case QNetworkReply::ContentReSendError:
            text += "The request needed to be sent again, but this failed for example because the upload data could not be read a second time.";
        break;
        case QNetworkReply::ContentConflictError:
            text += "The request could not be completed due to a conflict with the current state of the resource.";
        break;
        case QNetworkReply::ContentGoneError:
            text += "The requested resource is no longer available at the server.";
        break;
        case QNetworkReply::InternalServerError:
            text += "The server encountered an unexpected condition which prevented it from fulfilling the request.";
        break;
        case QNetworkReply::OperationNotImplementedError:
            text += "The server does not support the functionality required to fulfill the request.";
        break;
        case QNetworkReply::ServiceUnavailableError:
            text += "The server is unable to handle the request at this time.";
        break;
        case QNetworkReply::ProtocolUnknownError:
            text += "The Network Access API cannot honor the request because the protocol is not known";
        break;
        case QNetworkReply::ProtocolInvalidOperationError:
            text += "The requested operation is invalid for this protocol";
        break;
        case QNetworkReply::UnknownNetworkError:
            text += "An unknown network-related error was detected";
        break;
        case QNetworkReply::UnknownProxyError:
            text += "An unknown proxy-related error was detected";
        break;
        case QNetworkReply::UnknownContentError:
            text += "An unknown error related to the remote content was detected";
        break;
        case QNetworkReply::ProtocolFailure:
            text += "A breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)";
        break;
        case QNetworkReply::UnknownServerError:
            text += "An unknown error related to the server response was detected";
        break;
    }
    ui->textEdit->append(text);
}
