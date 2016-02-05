#include "BioXASSideBeamlineUtilities.h"

BioXASSideBeamlineUtilities::BioXASSideBeamlineUtilities(QObject *parent) :
    BioXASBeamlineUtilities(parent)
{
	// Pressure controls

	ccgSide1_ = new AMReadOnlyPVwStatusControl("Side pressure 1", "CCG1607-5-I00-03:vac:p", "CCG1607-5-I00-03:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide2_ = new AMReadOnlyPVwStatusControl("Side pressure 2", "CCG1607-5-I00-04:vac:p", "CCG1607-5-I00-04:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide3_ = new AMReadOnlyPVwStatusControl("Side pressure 3", "CCG1607-5-I00-05:vac:p", "CCG1607-5-I00-05:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide4_ = new AMReadOnlyPVwStatusControl("Side pressure 4", "CCG1607-5-I22-01:vac:p", "CCG1607-5-I22-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide5_ = new AMReadOnlyPVwStatusControl("Side pressure 5", "CCG1607-5-I22-02:vac:p", "CCG1607-5-I22-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide6_ = new AMReadOnlyPVwStatusControl("Side pressure 6", "CCG1607-5-I22-03:vac:p", "CCG1607-5-I22-03:vac", this, new AMControlStatusCheckerDefault(0));

	pressureSet_->addControl(ccgSide1_);
	pressureSet_->addControl(ccgSide2_);
	pressureSet_->addControl(ccgSide3_);
	pressureSet_->addControl(ccgSide4_);
	pressureSet_->addControl(ccgSide5_);
	pressureSet_->addControl(ccgSide6_);

	onPressureSetConnected();

	// Flow transducer controls

	flt5_ = new AMReadOnlyPVwStatusControl("Flow transducer 5", "FLT1607-5-I10-01", "FLT1607-5-I10-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt6_ = new AMReadOnlyPVwStatusControl("Flow transducer 6", "FLT1607-5-I10-02", "FLT1607-5-I10-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt7_ = new AMReadOnlyPVwStatusControl("Flow transducer 7", "FLT1607-5-I10-03", "FLT1607-5-I10-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt8_ = new AMReadOnlyPVwStatusControl("Flow transducer 8", "FLT1607-5-I10-04", "FLT1607-5-I10-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt9_ = new AMReadOnlyPVwStatusControl("Flow transducer 9", "FLT1607-5-I10-05", "FLT1607-5-I10-05:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt10_ = new AMReadOnlyPVwStatusControl("Flow transducer 10", "FLT1607-5-I10-06", "FLT1607-5-I10-06:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt11_ = new AMReadOnlyPVwStatusControl("Flow transducer 11", "FLT1607-5-I10-07", "FLT1607-5-I10-07:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt12_ = new AMReadOnlyPVwStatusControl("Flow transducer 12", "FLT1607-5-I20-01", "FLT1607-5-I20-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt13_ = new AMReadOnlyPVwStatusControl("Flow transducer 13", "FLT1607-5-I21-01", "FLT1607-5-I21-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt14_ = new AMReadOnlyPVwStatusControl("Flow transducer 14", "FLT1607-5-I21-02", "FLT1607-5-I21-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt15_ = new AMReadOnlyPVwStatusControl("Flow transducer 15", "FLT1607-5-I21-03", "FLT1607-5-I21-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt16_ = new AMReadOnlyPVwStatusControl("Flow transducer 16", "FLT1607-5-I21-04", "FLT1607-5-I21-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt17_ = new AMReadOnlyPVwStatusControl("Flow transducer 17", "FLT1607-5-I22-01", "FLT1607-5-I22-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt18_ = new AMReadOnlyPVwStatusControl("Flow transducer 18", "FLT1607-5-I22-02", "FLT1607-5-I22-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt19_ = new AMReadOnlyPVwStatusControl("Flow transducer 19", "FLT1607-5-I22-03", "FLT1607-5-I22-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt20_ = new AMReadOnlyPVwStatusControl("Flow transducer 20", "FLT1607-5-I22-04", "FLT1607-5-I22-04:lowflow", this, new AMControlStatusCheckerDefault(0));

	flowTransducerSet_->addControl(flt5_);
	flowTransducerSet_->addControl(flt6_);
	flowTransducerSet_->addControl(flt7_);
	flowTransducerSet_->addControl(flt8_);
	flowTransducerSet_->addControl(flt9_);
	flowTransducerSet_->addControl(flt10_);
	flowTransducerSet_->addControl(flt11_);
	flowTransducerSet_->addControl(flt12_);
	flowTransducerSet_->addControl(flt13_);
	flowTransducerSet_->addControl(flt14_);
	flowTransducerSet_->addControl(flt15_);
	flowTransducerSet_->addControl(flt16_);
	flowTransducerSet_->addControl(flt17_);
	flowTransducerSet_->addControl(flt18_);
	flowTransducerSet_->addControl(flt19_);
	flowTransducerSet_->addControl(flt20_);

	onFlowTransducerSetConnected();

	// Flow switch controls



	// Temperature controls
}

BioXASSideBeamlineUtilities::~BioXASSideBeamlineUtilities()
{

}
