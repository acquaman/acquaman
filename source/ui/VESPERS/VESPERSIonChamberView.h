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


#ifndef VESPERSIONCHAMBERVIEW_H
#define VESPERSIONCHAMBERVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>

#include "beamline/VESPERS/VESPERSIonChamber.h"

/// This class is a generic view that is very similar to the current view in the old user interface.  It has combo boxes for determining the sensitivity and a feedback value for the count rate.
class VESPERSIonChamberView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a VESPERSIonChamber and builds a view around it.
	explicit VESPERSIonChamberView(VESPERSIonChamber *ionChamber, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles passing changes in the value combo box to the ion chamber.
	void onValueComboBoxChanged(int index) { ionChamber_->setSensitivityValue(index); }
	/// Handles passing changes in the units combo box to the ion chamber.
	void onUnitsComboBoxChanged(int index) { ionChamber_->setSensitivityUnits(units_->itemText(index)); }

	/// Handles setting the value combo box when the ion chamber is changed from elsewhere.
	void onValueChanged(int value);
	/// Handles setting the units combo box when the ion chamber is changed from elsewhere.
	void onUnitsChanged(QString units);

protected:
	/// Pointer to the ion chamber being viewed.
	VESPERSIonChamber *ionChamber_;

	/// Combo box holding the value for the sensitivity.
	QComboBox *value_;
	/// Combo box holding the units for the sensitivity.
	QComboBox *units_;

};

/// This class is a simpler, more user friendly view than VESPERSIonChamberView.  It hides away the sensitivity choices and replaces them with - and + buttons with simple feedback of whether you're on target.
class VESPERSIonChamberBasicView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a VESPERSIonChamber and builds a view around it.  It also requires the acceptable voltage range to determine whether the ion chamber sensitivity is correct.
	explicit VESPERSIonChamberBasicView(VESPERSIonChamber *ionChamber, double min, double max, QWidget *parent = 0);

protected slots:
	/// Handles updates to the value of the ion chamber.
	void onIonChamberChanged();

protected:
	/// Pointer to the ion chamber being viewed.
	VESPERSIonChamber *ionChamber_;

	/// The minimum voltage for good sensitivity.
	double minimumVoltage_;
	/// The maximum voltage for good sensitivity.
	double maximumVoltage_;

	/// The visual feedback.  This will be either counts, voltage, or a word to tell you whether you are doing okay.
	QLabel *feedback_;
};

#include "beamline/VESPERS/VESPERSSplitIonChamber.h"

/// This class is a view that is meant to look like VESPERSIonChamberView but the only different is that it is for the split ion chamber.  Because they are not general yet, it needs it's own view class.
class VESPERSSplitIonChamberView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in a VESPERSSplitIonChamber and builds a view around it.
	explicit VESPERSSplitIonChamberView(VESPERSSplitIonChamber *ionChamber, QWidget *parent = 0);

protected slots:
	/// Handles passing changes in the value combo box to the ion chamber.
	void onValueComboBoxChanged(int index) { ionChamber_->setSensitivityValue(index); }
	/// Handles passing changes in the units combo box to the ion chamber.
	void onUnitsComboBoxChanged(int index) { ionChamber_->setSensitivityUnits(units_->itemText(index)); }

	/// Handles setting the value combo box when the ion chamber is changed from elsewhere.
	void onValueChanged(int value);
	/// Handles setting the units combo box when the ion chamber is changed from elsewhere.
	void onUnitsChanged(QString units);

protected:
	/// Pointer to the ion chamber being viewed.
	VESPERSSplitIonChamber *ionChamber_;

	/// Combo box holding the value for the sensitivity.
	QComboBox *value_;
	/// Combo box holding the units for the sensitivity.
	QComboBox *units_;
};

/// This class is a view that is meant to look like VESPERSIonChamberBasicView but the only different is that it is for the split ion chamber.  Because they are not general yet, it needs it's own view class.
class VESPERSSplitIonChamberBasicView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a VESPERSIonChamber and builds a view around it.  It also requires the acceptable voltage range to determine whether the ion chamber sensitivity is correct.
	explicit VESPERSSplitIonChamberBasicView(VESPERSSplitIonChamber *ionChamber, double min, double max, QWidget *parent = 0);

protected slots:
	/// Handles updates to the value of the ion chamber.
	void onIonChamberChanged();

protected:
	/// Pointer to the ion chamber being viewed.
	VESPERSSplitIonChamber *ionChamber_;

	/// The minimum voltage for good sensitivity.
	double minimumVoltage_;
	/// The maximum voltage for good sensitivity.
	double maximumVoltage_;

	/// The visual feedback.  This will be either counts, voltage, or a word to tell you whether you are doing okay.
	QLabel *feedback_;
};

#endif // VESPERSIONCHAMBERVIEW_H
