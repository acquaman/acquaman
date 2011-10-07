#include "CLSIonChamber.h"

CLSIonChamber::CLSIonChamber(const QString &name, const QString &description, const QString &countsPV, const QString &voltagePV, QObject *parent)
	: AMIonChamber(name, description, parent)
{
	counts_ = new AMReadOnlyPVControl(name+" counts", countsPV, this);
	voltage_ = new AMReadOnlyPVControl(name+" voltage", voltagePV, this);

	connect(counts_, SIGNAL(valueChanged(double)), this, SIGNAL(countsChanged(double)));
	connect(voltage_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageChanged(double)));

	/// ADD ION CHAMBER IS CONNECTED.  ALSO ADD TO AMIONCHAMBER!!!!!!!!
}
