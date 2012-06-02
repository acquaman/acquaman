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


#ifndef CLSIONCHAMBERVIEW_H
#define CLSIONCHAMBERVIEW_H

#include "ui/beamline/AMIonChamberView.h"
#include "beamline/CLS/CLSIonChamber.h"

#include <QComboBox>

/*!
  This class extends the AMIonChamberView by offering a more advanced view based on the CLSSR570 that is used to control the sensitivity.
  */
class CLSIonChamberView : public AMIonChamberView
{
	Q_OBJECT

public:
	/// Builds an ion chamber view that offers the ability to switch between the AMIonChamber "basic" view or the more advanced view that has greater control over the sensitivity by accessing the SR570 directly.
	CLSIonChamberView(CLSIonChamber *ionChamber, QWidget *parent = 0);

	/// Returns the a pointer to the chamber being viewed as a CLSIonChamber.  Returns 0 if an invalid chamber is being viewed.
	CLSIonChamber *ionChamberCLS() const { return qobject_cast<CLSIonChamber *>(chamber_); }

protected slots:
	/// Handles passing changes in the value combo box to the ion chamber.
	void onValueComboBoxChanged(int index) { ionChamberCLS()->setSensitivityValue(index); }
	/// Handles passing changes in the units combo box to the ion chamber.
	void onUnitsComboBoxChanged(int index) { ionChamberCLS()->setSensitivityUnits(units_->itemText(index)); }

	/// Handles setting the value combo box when the ion chamber is changed from elsewhere.
	void onValueChanged(int value);
	/// Handles setting the units combo box when the ion chamber is changed from elsewhere.
	void onUnitsChanged(QString units);

	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);
	/// Switches the view from basic to advanced or advanced to basic based on the current state of view flag.
	virtual void onModeViewChanged();

protected:
	/// Combo box holding the value for the sensitivity.
	QComboBox *value_;
	/// Combo box holding the units for the sensitivity.
	QComboBox *units_;

	/// Flag holding whether or not the view is basic or advanced.
	bool isBasic_;
};

#endif // CLSIONCHAMBERVIEW_H
