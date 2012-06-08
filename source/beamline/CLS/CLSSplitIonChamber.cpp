/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CLSSplitIonChamber.h"

CLSSplitIonChamber::CLSSplitIonChamber(const QString &name, const QString &description, const QString &countsAPV, const QString &countsBPV, const QString &voltageAPV, const QString &voltageBPV, const QString &sensitivityNameA, const QString &sensitivityNameB, const QString &sensitivityUnitsA, const QString &sensitivityUnitsB, QObject *parent)
	: AMSplitIonChamber(name, description, parent)
{
	connected_ = false;

	countsA_ = new AMReadOnlyPVControl(name + "A counts", countsAPV, this);
	countsB_ = new AMReadOnlyPVControl(name + "B counts", countsBPV, this);
	voltageA_ = new AMReadOnlyPVControl(name + "A voltage", voltageAPV, this);
	voltageB_ = new AMReadOnlyPVControl(name + "B voltage", voltageBPV, this);
	sensitivityA_ = new CLSSR570(sensitivityNameA, sensitivityUnitsA, this);
	sensitivityB_ = new CLSSR570(sensitivityNameB, sensitivityUnitsB, this);

	connect(countsA_, SIGNAL(valueChanged(double)), this, SIGNAL(countsAChanged(double)));
	connect(countsA_, SIGNAL(valueChanged(double)), this, SLOT(onCountsChanged()));
	connect(countsB_, SIGNAL(valueChanged(double)), this, SIGNAL(countsBChanged(double)));
	connect(countsB_, SIGNAL(valueChanged(double)), this, SLOT(onCountsChanged()));

	connect(voltageA_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageAChanged(double)));
	connect(voltageA_, SIGNAL(valueChanged(double)), this, SLOT(onVoltageChanged()));
	connect(voltageB_, SIGNAL(valueChanged(double)), this, SIGNAL(voltageBChanged(double)));
	connect(voltageB_, SIGNAL(valueChanged(double)), this, SLOT(onVoltageChanged()));

	connect(sensitivityA_, SIGNAL(valueChanged(int)), this, SIGNAL(sensitivityValueAChanged(int)));
	connect(sensitivityA_, SIGNAL(valueIndexChanged(int)), this, SIGNAL(sensitivityValueIndexAChanged(int)));
	connect(sensitivityB_, SIGNAL(valueChanged(int)), this, SIGNAL(sensitivityValueBChanged(int)));
	connect(sensitivityB_, SIGNAL(valueIndexChanged(int)), this, SIGNAL(sensitivityValueIndexBChanged(int)));
	connect(sensitivityA_, SIGNAL(unitsChanged(QString)), this, SIGNAL(sensitivityUnitsAChanged(QString)));
	connect(sensitivityB_, SIGNAL(unitsChanged(QString)), this, SIGNAL(sensitivityUnitsBChanged(QString)));
	connect(sensitivityA_, SIGNAL(minimumSensitivity(bool)), this, SIGNAL(minimumSensitivityA(bool)));
	connect(sensitivityB_, SIGNAL(minimumSensitivity(bool)), this, SIGNAL(minimumSensitivityB(bool)));
	connect(sensitivityA_, SIGNAL(maximumSensitivity(bool)), this, SIGNAL(maximumSensitivityA(bool)));
	connect(sensitivityB_, SIGNAL(maximumSensitivity(bool)), this, SIGNAL(maximumSensitivityB(bool)));

	connect(sensitivityA_, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityAChanged()));
	connect(sensitivityB_, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityBChanged()));

	connect(sensitivityA_, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityChanged()));
	connect(sensitivityB_, SIGNAL(sensitivityChanged()), this, SIGNAL(sensitivityChanged()));
}

void CLSSplitIonChamber::onConnectedChanged()
{
	bool isConnected = (countsA_->isConnected() && countsB_->isConnected()
					  && voltageA_->isConnected() && voltageB_->isConnected()
					  && sensitivityA_->isConnected() && sensitivityB_->isConnected());

	if (isConnected != connected_)
		emit connected(connected_ = isConnected);
}

void CLSSplitIonChamber::onCountsChanged()
{
	emit countsChanged();
	emit countsChanged(sum());
	emit readingsChanged();
}

void CLSSplitIonChamber::onVoltageChanged()
{
	emit voltageChanged();
	emit readingsChanged();
}
