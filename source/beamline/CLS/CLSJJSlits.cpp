#include "CLSJJSlits.h"
#include "beamline/AMSlit.h"
#include "beamline/CLS/CLSMAXvMotor.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent) :
	AMSlits(name, parent)
{
	// Initialize vertical slit blades.

	setUpperBlade(new CLSMAXvMotor(upperBladePVName, upperBladePVName, upperBladePVName, true, 0.005, 2.0, this), AMSlit::OpensNegatively);
	setLowerBlade(new CLSMAXvMotor(lowerBladePVName, lowerBladePVName, lowerBladePVName, true, 0.005, 2.0, this), AMSlit::OpensPositively);

	// Initialize horizontal slit blades.

	setInboardBlade(new CLSMAXvMotor(inboardBladePVName, inboardBladePVName, inboardBladePVName, true, 0.005, 2.0, this), AMSlit::OpensPositively);
	setOutboardBlade(new CLSMAXvMotor(outboardBladePVName, outboardBladePVName, outboardBladePVName, true, 0.005, 2.0, this), AMSlit::OpensNegatively);
}

CLSJJSlits::~CLSJJSlits()
{

}
