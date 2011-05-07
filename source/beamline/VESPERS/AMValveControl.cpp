#include "AMValveControl.h"

AMValveControl::AMValveControl(QString name, QString readPV, QString openPV, QString closePV, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	readPV_ = new AMProcessVariable(readPV, true, this);
	openPV_ = new AMProcessVariable(openPV, false, this);
	closePV_ = new AMProcessVariable(closePV, false, this);
	connect(readPV_, SIGNAL(valueChanged(int)), this, SIGNAL(stateChanged(int)));
}
