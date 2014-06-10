#ifndef AMADVANCEDCONTROLDETECTOREMULATOR_H
#define AMADVANCEDCONTROLDETECTOREMULATOR_H

#include "beamline/AMBasicControlDetectorEmulator.h"

#include "beamline/AMBeamline.h"

class AMAdvancedControlDetectorEmulator : public AMBasicControlDetectorEmulator
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as a pointer to the control you wish to acquire
	AMAdvancedControlDetectorEmulator(const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, const QString &synchronizedDwellKey, QObject *parent = 0);

	/// The cannot be configured in this manner
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns empty string
	virtual QString synchronizedDwellKey() const { return synchronizedDwellKey_;}

	/// No triggering source
	virtual bool sharesDetectorTriggerSource() { return true; }
	/// Returns Null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return AMBeamline::bl()->synchronizedDwellTime()->triggerSource(); }

public slots:
	void setSynchronizedDwellKey(const QString &synchronizedDwellKey) { synchronizedDwellKey_ = synchronizedDwellKey; }

protected:
	QString synchronizedDwellKey_;
};

#endif // AMADVANCEDCONTROLDETECTOREMULATOR_H
