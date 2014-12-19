#include <iostream>
#include <string>
#include <thread>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "MainWindow.h"
#include "QtPicDriver.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("BullyCPP");

	// Parse options

	QCommandLineParser parser;
	parser.setApplicationDescription("Flashes code to PIC devices running the Bully Bootloader.");
	parser.addHelpOption();
	parser.addOption(QCommandLineOption({"r", "mclr"},
		"Assert MCLR (RTS/DTR line) before programming target"));
	parser.addOption(QCommandLineOption({"c", "configbits"},
		"Program config bits"));
	parser.addOption(QCommandLineOption({"D", "device"},
		"Serial device to use",
		"device"));
	parser.addOption(QCommandLineOption({"b", "baud"},
		"Serial port speed",
		"baud", "230400"));
	parser.addOption(QCommandLineOption("piclist",
		"PIC device file to read", "devices.txt"));
	parser.addOption(QCommandLineOption("no-gui",
		"Do not show GUI"));
	parser.addPositionalArgument("hexfile",
		"The Intel hex file to send (required with --no-gui)", "[hexfile]");
	parser.process(app);

	if(!parser.isSet("no-gui")) {
		MainWindow w;
		w.show();

		return app.exec();
	}
	else if(!parser.positionalArguments().size() || !parser.isSet("device")) {
		parser.showHelp(1);
	}

	// Do flash

	try {
		std::cout << "Initializing... ";

		QtPicDriver driver(parser.values("piclist"));
		driver.setMCLROnProgram(parser.isSet("mclr"));
		driver.setConfigBitsEnabled(parser.isSet("configbits"));

		std::cout << "OK\n";

		driver.programHexFile(parser.positionalArguments()[0]);
	}
	catch(std::exception& e) {
		std::cout << argv[0] << ": " << e.what() << std::endl;
	}

	return 0;
}
