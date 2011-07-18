#include "CLSSynchronizedDwellTime.h"

CLSSynchronizedDwellTime::CLSSynchronizedDwellTime(QString baseName, QObject *parent)
	: QObject(parent)
{
	baseName_ = baseName;

	dwellTime_ = new AMProcessVariable(baseName+":setTime", true, this);
	startScan_ = new AMProcessVariable(baseName+":startScan", true, this);
	mode_ = new AMProcessVariable(baseName+":setMode", true, this);

	connect(dwellTime_, SIGNAL(valueChanged(double)), this, SIGNAL(timeChanged(double)));
	connect(startScan_, SIGNAL(valueChanged(double)), this, SLOT(onScanningChanged(int)));
	connect(mode_, SIGNAL(valueChanged(int)), this, SLOT(onModeChanged(int)));
}

void CLSSynchronizedDwellTime::addElement(int index)
{
	elements_ << new CLSSynchronizedDwellTimeElement(baseName_, index, this);
	connect(elements_.last(), SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged()));
}

CLSSynchronizedDwellTimeElement::CLSSynchronizedDwellTimeElement(QString baseName, int index, QObject *parent)
	: QObject(parent)
{
	// 65 is 'A' in ascii.  Therefore the index offset will give the appropriate letter for the PV name since they are named 'A', 'B', 'C', etc.
	name_ = new AMProcessVariable(baseName+":device"+QChar(65+index), true, this);
	enable_ = new AMProcessVariable(baseName+":enable"+QChar(65+index), true, this);
	time_ = new AMProcessVariable(baseName+":set"+QChar(65+index), true, this);
	status_ = new AMProcessVariable(baseName+":status"+QChar(65+index), true, this);

	connect(name_, SIGNAL(valueChanged(QString)), this, SIGNAL(nameChanged(QString)));
	connect(time_, SIGNAL(valueChanged(double)), this, SIGNAL(timeChanged(double)));
	connect(enable_, SIGNAL(valueChanged(int)), this, SLOT(onEnabledChanged(int)));
	connect(status_, SIGNAL(valueChanged(int)), this, SLOT(onStatusChanged(int)));
}
