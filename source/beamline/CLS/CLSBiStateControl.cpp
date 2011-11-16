#include "CLSBiStateControl.h"

CLSBiStateControl::CLSBiStateControl(const QString &name, const QString &description, const QString &state, const QString &open, const QString &close, AMAbstractControlStatusChecker *statusChecker, QObject *parent)
	: AMControl(name, "n/a", parent, description)
{
	setTolerance(0.1);

	statusChecker_ = statusChecker;

	statePV_ = new AMProcessVariable(state, true, this);
	openPV_ = new AMProcessVariable(open, true, this);
	closePV_ = new AMProcessVariable(close, true, this);

	connect(statePV_, SIGNAL(valueChanged(int)), this, SIGNAL(stateChanged(int)));
}
