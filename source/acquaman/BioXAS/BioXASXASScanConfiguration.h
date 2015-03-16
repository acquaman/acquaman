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


#ifndef BIOXASXASSCANCONFIGURATION_H
#define BIOXASXASSCANCONFIGURATION_H

#include <QObject>

#include "acquaman/AMStepScanConfiguration.h"

class BioXASXASScanConfiguration : public AMStepScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS XAS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original);
	///Destructor.
	virtual ~BioXASXASScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	double edgeEnergy() const;

	/// Returns the name of the current edge.
	QString edge() const { return edge_; }

signals:
	void edgeEnergyChanged(double edgeEnergy);
	void edgeChanged(const QString &edge);

public slots:
	void setEdgeEnergy(double edgeEnergy);

	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);

protected:
	double edgeEnergy_;
	/// The edge being scanned.
	QString edge_;

};

#endif // BIOXASXASSCANCONFIGURATION_H
