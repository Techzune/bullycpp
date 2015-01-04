/** This file is part of BullyCPP.
 **
 **     BullyCPP is free software: you can redistribute it and/or modify
 **     it under the terms of the GNU General Public License as published by
 **     the Free Software Foundation, either version 3 of the License, or
 **     (at your option) any later version.
 **
 **     BullyCPP is distributed in the hope that it will be useful,
 **     but WITHOUT ANY WARRANTY; without even the implied warranty of
 **     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **     GNU General Public License for more details.
 **
 **     You should have received a copy of the GNU General Public License
 **     along with BullyCPP.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "QtPicBootloaderDriver.h"

#include <QFile>
#include <QMap>

#include "QStdStreamBuf.h"
#include "bullycpp/PicDevice.h"

QtPicBootloaderDriver::QtPicBootloaderDriver(bullycpp::ISerialPort& serialPort,QObject *parent)
	: QObject(parent)
	, driver(serialPort, this)
{}

QtPicBootloaderDriver::~QtPicBootloaderDriver()
{}

void QtPicBootloaderDriver::setConfigBitsEnabled(bool enabled)
{
	driver.setConfigBitsEnabled(enabled);
}

bool QtPicBootloaderDriver::configBitsEnabled()
{
	return driver.getConfigBitsEnabled();
}

void QtPicBootloaderDriver::programHexFile(const QString path)
{
	driver.programHexFile(path.toStdString());
}

void QtPicBootloaderDriver::parseDeviceFile(const QString path)
{
	QFile file(path);
	if(file.open(QIODevice::ReadOnly)) {
		QStdIStream stream(&file);
		driver.parseDeviceFile(stream);
	}
}

void QtPicBootloaderDriver::setMCLR(bool mclr)
{
	driver.setMCLR(mclr);
}

bool QtPicBootloaderDriver::readDevice()
{
	const bullycpp::PicDevice* optionalDevice = driver.readDevice();
	if(optionalDevice) {
		emit deviceChanged(QString::fromStdString(optionalDevice->name));
		return true;
	}
	else return false;
}

void QtPicBootloaderDriver::onProgress(Status status, int percent)
{
	emit programmingStatusChanged(status, percent);
}
