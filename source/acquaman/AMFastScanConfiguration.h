/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMFASTSCANCONFIGURATION_H
#define AMFASTSCANCONFIGURATION_H

#include "AMScanConfiguration.h"

class AMFastScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double startEnergy READ startEnergy WRITE setStartEnergy)
	Q_PROPERTY(double endEnergy READ endEnergy WRITE setEndEnergy)
	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Fast Scan Configuration")

public:
 	virtual ~AMFastScanConfiguration();
	AMFastScanConfiguration(QObject *parent = 0);
	AMFastScanConfiguration(const AMFastScanConfiguration &original);

	const QMetaObject* getMetaObject();

	double startEnergy() const { return startEnergy_;}
	double endEnergy() const { return endEnergy_;}

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		return QString("Fast Scan from %1 to %2").arg(startEnergy()).arg(endEnergy());
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("Fast Scan from %1 to %2").arg(startEnergy()).arg(endEnergy());
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("Fast Scan");
	}

public slots:
	bool setStartEnergy(double startEnergy);
	bool setEndEnergy(double endEnergy);

signals:
	void energyRangeChanged();

protected:
	double startEnergy_;
	double endEnergy_;
};

#endif // AMFASTSCANCONFIGURATION_H
