#include "CLSJJSlit.h"

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	name_ = name;
	connected_ = false;

//	upperBlade_ = new CLSMAXvMotor(name_+"UpperBlade", upperBladePVName, name_+" UpperBlade", true, 0.005, 10.0, this, "mm");
//	lowerBlade_ = new CLSMAXvMotor(name_+"LowerBlade", lowerBladePVName, name_+" LowerBlade", true, 0.005, 10.0, this, "mm");
//	inboardBlade_ = new CLSMAXvMotor(name_+"InboardBlade", inboardBladePVName, name_+" InboardBlade", true, 0.005, 10.0, this, "mm");
//	outboardBlade_ = new CLSMAXvMotor(name_+"OutboardBlade", outboardBladePVName, name_+" OutboardBlade", true, 0.005, 10.0, this, "mm");

	upperBlade_ = new AMPVwStatusControl(name_+"UpperBlade", upperBladePVName+":mm:fbk", upperBladePVName+":mm", upperBladePVName+":status", upperBladePVName+":stop", this, 0.005);
	lowerBlade_ = new AMPVwStatusControl(name_+"LowerBlade", lowerBladePVName+":mm:fbk", lowerBladePVName+":mm", lowerBladePVName+":status", lowerBladePVName+":stop", this, 0.005);
	inboardBlade_ = new AMPVwStatusControl(name_+"InboardBlade", inboardBladePVName+":mm:fbk", inboardBladePVName+":mm", inboardBladePVName+":status", inboardBladePVName+":stop", this, 0.005);
	outboardBlade_ = new AMPVwStatusControl(name_+"OutboardBlade", outboardBladePVName+":mm:fbk", outboardBladePVName+":mm", outboardBladePVName+":status", outboardBladePVName+":stop", this, 0.005);


	verticalGap_ = new CLSJJSlitGapControl(name_+"VerticalGap", upperBlade_, lowerBlade_, this);
	verticalCenter_ = new CLSJJSlitCenterControl(name_+"VerticalCenter", upperBlade_, lowerBlade_, this);
	horizontalGap_ = new CLSJJSlitGapControl(name_+"HorizontalGap", outboardBlade_, inboardBlade_, this);
	horizontalCenter_ = new CLSJJSlitCenterControl(name_+"HorizontalCenter", outboardBlade_, inboardBlade_, this);

	// Listen for connected states.

	connect( verticalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
}

CLSJJSlits::~CLSJJSlits()
{

}

void CLSJJSlits::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void CLSJJSlits::updateConnected()
{
	bool newState = verticalGap_->isConnected() && verticalCenter_->isConnected() && horizontalGap_->isConnected() && horizontalCenter_->isConnected();
	setConnected(newState);
}
