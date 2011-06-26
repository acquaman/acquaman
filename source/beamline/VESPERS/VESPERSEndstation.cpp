#include "VESPERSEndstation.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

VESPERSEndstation::VESPERSEndstation(QObject *parent)
	: QObject(parent)
{
	current_ = 0;
	wasConnected_ = false;

	// The controls.
	ccdControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->ccdMotor());
	microscopeControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->microscopeMotor());
	fourElControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->fourElMotor());
	singleElControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->singleElMotor());
	focusControl_ = VESPERSBeamline::vespers()->sampleStage()->norm();

	// Laser power control.
	laserPower_ = qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->laserPower());

	// The feedback PVs associated with the controls.
	ccdfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->ccdMotorfbk());
	fourElfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->fourElMotorfbk());
	singleElfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->singleElMotorfbk());
	focusfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->focusMotorfbk());

	// The microscope light and CCD file path PVs.
	micLightPV_ = VESPERSBeamline::vespers()->micLight();
	ccdPath_ = VESPERSBeamline::vespers()->ccdPath();
	ccdFile_ = VESPERSBeamline::vespers()->ccdFile();
	ccdNumber_ = VESPERSBeamline::vespers()->ccdNumber();

	// Pseudo-motor reset button.
	resetPseudoMotors_ = VESPERSBeamline::vespers()->resetPseudoMotors();

	// Setup filters.
	filterMap_.insert("50A", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter50umA()));
	filterMap_.insert("50B", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter50umB()));
	filterMap_.insert("100A", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter100umA()));
	filterMap_.insert("100B", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter100umB()));
	filterMap_.insert("250A", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter250umA()));
	filterMap_.insert("250B", qobject_cast<AMPVControl *>(VESPERSBeamline::vespers()->filter250umB()));

	// Get the current soft limits.
	loadConfiguration();

	// Connections.
	connect(laserPower_, SIGNAL(valueChanged(double)), this, SIGNAL(laserPoweredChanged()));
	connect(micLightPV_, SIGNAL(valueChanged(int)), this, SIGNAL(lightIntensityChanged(int)));
	connect(ccdfbk_, SIGNAL(valueChanged(double)), this, SIGNAL(ccdFbkChanged(double)));
	connect(microscopeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(microscopeFbkChanged(double)));
	connect(focusfbk_, SIGNAL(valueChanged(double)), this, SIGNAL(focusFbkChanged(double)));
	connect(singleElfbk_, SIGNAL(valueChanged(double)), this, SIGNAL(singleElFbkChanged(double)));
	connect(fourElfbk_, SIGNAL(valueChanged(double)), this, SIGNAL(fourElFbkChanged(double)));
	connect(ccdPath_, SIGNAL(valueChanged()), this, SLOT(onCCDPathChanged()));
	connect(ccdFile_, SIGNAL(valueChanged()), this, SLOT(onCCDNameChanged()));
	connect(ccdNumber_, SIGNAL(valueChanged(int)), this, SIGNAL(ccdNumberChanged(int)));

	QList<AMPVControl *> list(filterMap_.values());
	for (int i = 0; i < list.size(); i++)
		connect(list.at(i), SIGNAL(connected(bool)), this, SLOT(onFiltersConnected()));
}

bool VESPERSEndstation::loadConfiguration()
{
	QFile file(QDir::currentPath() + "/endstation.config");

	if (!file.open(QFile::ReadOnly | QFile::Text))
		return false;

	QTextStream in(&file);
	QStringList contents;

	while(!in.atEnd())
		contents << in.readLine();

	file.close();

	softLimits_.clear();

	softLimits_.insert(ccdControl_, qMakePair(((QString)contents.at(2)).toDouble(), ((QString)contents.at(3)).toDouble()));
	softLimits_.insert(singleElControl_, qMakePair(((QString)contents.at(6)).toDouble(), ((QString)contents.at(7)).toDouble()));
	softLimits_.insert(fourElControl_, qMakePair(((QString)contents.at(10)).toDouble(), ((QString)contents.at(11)).toDouble()));
	softLimits_.insert(microscopeControl_, qMakePair(((QString)contents.at(14)).toDouble(), ((QString)contents.at(15)).toDouble()));

	microscopeNames_ = qMakePair((QString)contents.at(16), (QString)contents.at(17));

	updateControl(current_);

	return true;
}

AMPVwStatusControl *VESPERSEndstation::control(QString name) const
{
	if (name.compare("CCD motor") == 0)
		return ccdControl_;
	else if (name.compare("1-Element Vortex motor") == 0)
		return singleElControl_;
	else if (name.compare("4-Element Vortex motor") == 0)
		return fourElControl_;
	else if (name.compare("Microscope motor") == 0)
		return microscopeControl_;
	else if (name.compare("Normal Sample Stage") == 0)
		return focusControl_;

	return 0;
}

void VESPERSEndstation::setCurrent(QString name)
{
	if (name.compare("CCD motor") == 0)
		updateControl(ccdControl_);
	else if (name.compare("1-Element Vortex motor") == 0)
		updateControl(singleElControl_);
	else if (name.compare("4-Element Vortex motor") == 0)
		updateControl(fourElControl_);
	else if (name.compare("Microscope motor") == 0)
		updateControl(microscopeControl_);
	else if (name.compare("Normal Sample Stage") == 0)
		updateControl(focusControl_);
}

void VESPERSEndstation::updateControl(AMPVwStatusControl *control)
{
	if (control == 0)
		return;

	current_ = control;
	emit currentControlChanged(current_);
}

void VESPERSEndstation::onFiltersConnected()
{
	bool connected = true;
	QList<AMPVControl *> filters(filterMap_.values());

	for (int i = 0; i < filters.size(); i++)
		connected = connected && filters.at(i)->isConnected();

	if (!wasConnected_ && connected){

		for (int i = 0; i < filters.size(); i++)
			connect(filters.at(i), SIGNAL(valueChanged(double)), this, SLOT(onFiltersChanged()));

		onFiltersChanged();
	}
	else if (wasConnected_ && !connected)
		for (int i = 0; i < filters.size(); i++)
			disconnect(filters.at(i), SIGNAL(valueChanged(double)), this, SLOT(onFiltersChanged()));
}

void VESPERSEndstation::onFiltersChanged()
{
	int sum = 0;

	if ((int)filterMap_.value("50A")->value() == 1)
		sum += 1;
	if ((int)filterMap_.value("50B")->value() == 1)
		sum += 1;
	if ((int)filterMap_.value("100A")->value() == 1)
		sum += 2;
	if ((int)filterMap_.value("100B")->value() == 1)
		sum += 2;
	if ((int)filterMap_.value("250A")->value() == 1)
		sum += 5;
	if ((int)filterMap_.value("250B")->value() == 1)
		sum += 5;

	emit filterThicknessChanged(sum);
}

void VESPERSEndstation::setFilterThickness(int index)
{
	QList<AMPVControl *> filters(filterMap_.values());

	// Put all the filters back to an original state.  The -2 is to exclude the upper and lower shutters.
	for (int i = 0; i < filters.size(); i++)
		if ((int)filters.at(i)->value() == 1)
			toggleControl(filters.at(i));

	switch(index){
	case 0: // Filters are already taken out with previous loop.
		break;
	case 1: // 50 um
		toggleControl(filterMap_.value("50A"));
		break;
	case 2: // 100 um
		toggleControl(filterMap_.value("100A"));
		break;
	case 3: // 150 um
		toggleControl(filterMap_.value("50A"));
		toggleControl(filterMap_.value("100A"));
		break;
	case 4: // 200 um
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("100B"));
		break;
	case 5: // 250 um
		toggleControl(filterMap_.value("250A"));
		break;
	case 6: // 300 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("50A"));
		break;
	case 7: // 350 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("100A"));
		break;
	case 8: // 400 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("50A"));
		break;
	case 9: // 450 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("100B"));
		break;
	case 10: // 500 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		break;
	case 11: // 550 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("50A"));
		break;
	case 12: // 600 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("100A"));
		break;
	case 13: // 650 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("50A"));
		break;
	case 14: // 700 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("100B"));
		break;
	case 15: // 750 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("100B"));
		toggleControl(filterMap_.value("50A"));
		break;
	case 16: // 800 um
		toggleControl(filterMap_.value("250A"));
		toggleControl(filterMap_.value("250B"));
		toggleControl(filterMap_.value("100A"));
		toggleControl(filterMap_.value("100B"));
		toggleControl(filterMap_.value("50A"));
		toggleControl(filterMap_.value("50B"));
		break;
	}
}

QString VESPERSEndstation::AMPVtoString(AMProcessVariable *pv)
{
	int current;
	QString name;

	for (unsigned i = 0; i < pv->count(); i++){

		current = pv->getInt(i);
		if (current == 0)
			break;

		name += QString::fromAscii((const char *) &current);
	}

	return name;
}

void VESPERSEndstation::StringtoAMPV(AMProcessVariable *pv, QString toConvert)
{
	int converted[256];

	for (int i = 0; i < 256; i++){

		if (i < toConvert.size())
			converted[i] = toConvert.toAscii()[i];
		else
			converted[i] = 0;
	}

	pv->setValues(converted, 256);
}
