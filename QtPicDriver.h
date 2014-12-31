#ifndef QTPICDRIVER_H
#define QTPICDRIVER_H

#include <atomic>

#include <QObject>

#include "bullycpp/IProgressCallback.h"
#include "SerialPort.h"
#include "QtPicBootloaderDriver.h"

class QtPicDriver : public QObject
{
	Q_OBJECT
public:
	explicit QtPicDriver(const QStringList& deviceFiles, QObject *parent = 0);
	virtual ~QtPicDriver();

signals:
	void configBitsEnabledChanged(bool);
	void deviceChanged(QString);
	void serialDataReceived(QByteArray);
	void mclrChanged(bool);
	void serialPortStatusChanged(bool);
	void serialPortErrorChanged(QString);
	void programmingStateChanged(bool);
	void programmingErrorChanged(bool);
	void programmingProgressChanged(QString, int percent);

public slots:
	void sendSerialData(const QByteArray data);
	void programHexFile(const QString path);
	void setSerialPort(const QString name);
	void setBaudRate(const QString baud);
	void setMCLR(bool mclr);
	void setMCLROnProgram(bool set);
	void setConfigBitsEnabled(bool set);
	void openSerialPort();

private slots:
	void onReadyRead();
	void onDeviceChanged(const QString device);
	void onProgrammingStatusChanged(bullycpp::IProgressCallback::Status status, int percent);

private:
	SerialPort serialPort;
	QtPicBootloaderDriver bootloaderDriver;
	std::atomic<bool> forwardData;
	bool mclrOnProgram;
};

#endif // QTPICDRIVER_H
