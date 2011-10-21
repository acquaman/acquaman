#include "AMSplitIonChamber.h"

AMSplitIonChamber::AMSplitIonChamber(const QString &name, const QString &description, QObject *parent)
	: AMIonChamber(name, description, parent)
{
	// If either of the sensitivities max/min out then the overall signal should be emitted.
	connect(this, SIGNAL(minimumSensitivityA(bool)), this, SIGNAL(minimumSensitivity(bool)));
	connect(this, SIGNAL(minimumSensitivityB(bool)), this, SIGNAL(minimumSensitivity(bool)));

	connect(this, SIGNAL(maximumSensitivityA(bool)), this, SIGNAL(maximumSensitivity(bool)));
	connect(this, SIGNAL(maximumSensitivityB(bool)), this, SIGNAL(maximumSensitivity(bool)));
}
