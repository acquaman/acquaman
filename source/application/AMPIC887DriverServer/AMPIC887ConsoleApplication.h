#ifndef AMPIC887CONSOLEAPPLICATION_H
#define AMPIC887CONSOLEAPPLICATION_H

#include <QCoreApplication>

#include "AMPIC887Controller.h"
#include "AMPIC887ControllerCollection.h"

class AMConsoleInputHandler;
class AMPIC887ConsoleCommandParser;
class AMPIC887ConsoleApplication : public QCoreApplication
{
    Q_OBJECT
public:
	explicit AMPIC887ConsoleApplication(int argc, char *argv[]);

signals:

public slots:
protected slots:
	void onHelpCommandIssued();
	void onQuitCommandIssued();
	void onGCS2CommandIssued(AMGCS2Command* command);
protected:
	bool startup();
	bool startupControllers();
	AMConsoleInputHandler* consoleApplication_;
	AMPIC887ConsoleCommandParser* commandParser_;
	AMPIC887ControllerCollection controllerCollection_;
};

#endif // AMPIC887CONSOLEAPPLICATION_H
