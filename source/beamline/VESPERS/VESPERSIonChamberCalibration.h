/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSIONCHAMBERCALIBRATION_H
#define VESPERSIONCHAMBERCALIBRATION_H

#include <QObject>
#include <QList>

#include "beamline/VESPERS/VESPERSIonChamber.h"
#include "beamline/VESPERS/VESPERSSplitIonChamber.h"

/*!
  This class gives access to calibration of the ion chambers on the VESPERS beamline.  This essenstially acts like a container class.

	\todo This also needs to be updated.  If I can make the ion chamber a more general class, this could be migrated to a pre-existing
	container class or generalized properly.  As of right now, this class is now a bastardized hodge-podged class that needs a general
	class to be able to operate correctly.
  */
class VESPERSIonChamberCalibration : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds the VESPERS specific ion chamber calibration model.
	explicit VESPERSIonChamberCalibration(QObject *parent = 0);

	/// Add a split ion chamber.
	void addSplitIonChamber(VESPERSSplitIonChamber *splitIonChamber) { splitIonChamber_ = splitIonChamber; }
	/// Remove the split ion chamber.
	void removeSplitIonChamber() { splitIonChamber_ = 0; }
	/// Returns the split ion chamber.  Returns 0 if not set.
	VESPERSSplitIonChamber *splitIonChamber() const { return splitIonChamber_; }

	/// Appends a VESPERSIonChamber to the end of the list.
	void addIonChamber(VESPERSIonChamber *newIonChamber) { ionChambers_ << newIonChamber; }
	/// Removes a VESPERSIonChamber from the list based on the name.  Does NOT delete the ion chamber.
	void removeIonChamber(QString name)
	{
		for (int i = 0; i < ionChambers_.size(); i++)
			if (name == ionChambers_.at(i)->name())
				ionChambers_.removeAt(i);
	}
	/// Returns the number of ion chambers.  Does NOT include the split ion chamber.
	int ionChamberCount() const { return ionChambers_.size(); }

	/// Returns the ion chamber from \param index.  Must be between 0 and ionChamberCount().
	VESPERSIonChamber *ionChamberAt(int index) const { return ionChambers_.at(index); }

signals:

public slots:

protected:

	/// A list holding VESPERSIonChambers.  Does NOT include the split ion chamber.
	QList<VESPERSIonChamber *> ionChambers_;
	/// The split ion chamber.
	VESPERSSplitIonChamber *splitIonChamber_;
};

#endif // VESPERSIONCHAMBERCALIBRATION_H
