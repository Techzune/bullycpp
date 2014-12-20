#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	picDriver(new QtPicDriver(nullptr))
{
	ui->setupUi(this);

	ui->progressWidget->hide();

	thread.start();
	picDriver->moveToThread(&thread);

	fileDialog.setNameFilters({"Intel Hex files (*.hex)", "All files (*)"});

	connect(ui->programButton, SIGNAL(clicked()), SLOT(onProgramButtonClicked()));
	connect(ui->hexFileNameEdit, SIGNAL(textChanged(QString)), SLOT(onHexFileTextChanged(QString)));
	connect(this, SIGNAL(programHexFile(QString)), picDriver, SLOT(programHexFile(QString)));
	connect(picDriver, SIGNAL(deviceChanged(QString)), ui->deviceInfoLabel, SLOT(setText(QString)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->progressWidget, SLOT(setVisible(bool)));
	connect(picDriver, SIGNAL(programmingStateChanged(bool)), ui->programmingWidget, SLOT(setHidden(bool)));

	connect(ui->chooseHexFileButton, SIGNAL(clicked()), &fileDialog, SLOT(open()));
	connect(&fileDialog, SIGNAL(fileSelected(QString)), ui->hexFileNameEdit, SLOT(setText(QString)));

	connect(ui->mclrCheckBox, SIGNAL(clicked(bool)), picDriver, SLOT(setMCLROnProgram(bool)));

	connect(ui->configBitCheckBox, SIGNAL(clicked(bool)), picDriver, SLOT(setConfigBitsEnabled(bool)));

	connect(ui->mclrButton, SIGNAL(clicked(bool)), picDriver, SLOT(setMCLR(bool)));
	connect(picDriver, SIGNAL(mclrChanged(bool)), ui->mclrButton, SLOT(setChecked(bool)));

	connect(ui->serialText, SIGNAL(keyPressed(QString)), SLOT(onSerialTextSend(QString)));
	connect(picDriver, SIGNAL(serialDataReceived(QByteArray)), SLOT(onSerialTextReceived(QByteArray)));
	connect(this, SIGNAL(sendSerialData(QByteArray)), picDriver, SLOT(sendSerialData(QByteArray)));

	connect(ui->clearSerialButton, SIGNAL(clicked()), ui->serialText, SLOT(clear()));

	connect(ui->serialPortComboBox, SIGNAL(currentIndexChanged(QString)), picDriver, SLOT(setSerialPort(QString)));
	connect(ui->baudComboBox, SIGNAL(currentTextChanged(QString)), picDriver, SLOT(setBaudRate(QString)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialText, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->serialErrorWidget, SLOT(setHidden(bool)));
	connect(picDriver, SIGNAL(serialPortStatusChanged(bool)), ui->mclrButton, SLOT(setEnabled(bool)));
	connect(picDriver, SIGNAL(serialPortErrorChanged(QString)), ui->serialStatusLabel, SLOT(setText(QString)));
	connect(ui->retrySerialButton, SIGNAL(clicked()), picDriver, SLOT(openSerialPort()));
	for(const auto& port: QSerialPortInfo::availablePorts()) {
		ui->serialPortComboBox->addItem(port.portName());
	}
	for(const auto& baud: QSerialPortInfo::standardBaudRates()) {
		QString baudAsString;
		baudAsString.setNum(baud);
		ui->baudComboBox->addItem(baudAsString);
	}
	ui->baudComboBox->addItem("230400");
	ui->baudComboBox->setCurrentText("230400");
}

MainWindow::~MainWindow()
{
	picDriver->deleteLater();
	thread.exit();
	thread.wait();

	delete ui;
}

void MainWindow::onProgramButtonClicked()
{
	emit programHexFile(ui->hexFileNameEdit->text());
}

void MainWindow::onSerialTextSend(QString text)
{
	emit sendSerialData(text.toLocal8Bit());
}

void MainWindow::onSerialTextReceived(QByteArray data)
{
	ui->serialText->moveCursor(QTextCursor::End);
	ui->serialText->insertPlainText(data);
	ui->serialText->moveCursor(QTextCursor::End);
}

void MainWindow::onHexFileTextChanged(QString text)
{
	// Only makes sense to enable the programButton if serial is enabled
	ui->programButton->setEnabled(text.size() != 0 && ui->serialText->isEnabled());
}
