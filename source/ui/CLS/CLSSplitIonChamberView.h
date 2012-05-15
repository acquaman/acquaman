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


#ifndef CLSSPLITIONCHAMBERVIEW_H
#define CLSSPLITIONCHAMBERVIEW_H

#include "beamline/CLS/CLSSplitIonChamber.h"
#include "ui/beamline/AMSplitIonChamberView.h"

#include <QComboBox>

/*!
  This class extends the AMSplitIonChamberView by offering a more advanced view based on the CLSSR570 that is used to control the sensitivity.
  */
class CLSSplitIonChamberView : public AMSplitIonChamberView
{
	Q_OBJECT

public:
	/// Builds an ion chamber view that offers the ability to switch between the AMSplitIonChamber "basic" view or the more advanced view that has greater control over the sensitivity by accessing the SR570 directly.
	CLSSplitIonChamberView(CLSSplitIonChamber *chamber, QWidget *parent = 0);

	/// Returns the a pointer to the chamber being viewed as a CLSSplitIonChamber.  Returns 0 if an invalid chamber is being viewed.
	CLSSplitIonChamber *splitIonChamberCLS() const { return qobject_cast<CLSSplitIonChamber *>(chamber_); }

protected slots:
	/// Handles passing changes in the value combo box to both sides of the ion chamber.
	void onValueComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityValueA(index); splitIonChamberCLS()->setSensitivityValueB(index); }
	/// Handles passing changes in the units combo box to both sides of the ion chamber.
	void onUnitsComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityUnitsA(unitsA_->itemText(index)); splitIonChamberCLS()->setSensitivityUnitsB(unitsB_->itemText(index)); }

	/// Handles passing changes in the value combo box to one side of the ion chamber (A).
	void onValueAComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityValueA(index); }
	/// Handles passing changes in the units combo box to one side of the ion chamber (A).
	void onUnitsAComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityUnitsA(unitsA_->itemText(index)); }

	/// Handles passing changes in the value combo box to the other side of the ion chamber (B).
	void onValueBComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityValueB(index); }
	/// Handles passing changes in the units combo box to the other side of the ion chamber (B).
	void onUnitsBComboBoxChanged(int index) { splitIonChamberCLS()->setSensitivityUnitsB(unitsB_->itemText(index)); }

	/// Handles setting the value combo box for both sides of the ion chamber from elsewhere.
	void onValueChanged(int value);
	/// Handles setting the value combo box for both sides of the ion chamber from elsewhere.
	void onUnitsChanged(QString units);

	/// Handles setting the value combo box when one side of the ion chamber (A) is changed from elsewhere.
	void onValueAChanged(int value) { 	setValueComboBox(valueA_, value); }
	/// Handles setting the units combo box when one side of the ion chamber (A) is changed from elsewhere.
	void onUnitsAChanged(QString units) { setUnitsComboBox(unitsA_, units); }

	/// Handles setting the value combo box when the other side of the ion chamber (B) is changed from elsewhere.
	void onValueBChanged(int value) { setValueComboBox(valueB_, value); }
	/// Handles setting the units combo box when the other side of the ion chamber (B) is changed from elsewhere.
	void onUnitsBChanged(QString units) { setUnitsComboBox(unitsB_, units); }

	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);
	/// Switches the view from basic to advanced or advanced to basic based on the current state of view flag.
	virtual void onModeViewChanged();

protected:
	/// Combo box holding the value for the sensitivity.
	QComboBox *value_;
	/// Combo box holding the units for the sensitivity.
	QComboBox *units_;

	/// Combo box holding the value for the sensitivity for one side of the ion chamber (A).
	QComboBox *valueA_;
	/// Combo box holding the units for the sensitivity for one side of the ion chamber (A).
	QComboBox *unitsA_;
	/// Combo box holding the value for the sensitivity for one side of the ion chamber (B).
	QComboBox *valueB_;
	/// Combo box holding the units for the sensitivity for one side of the ion chamber (B).
	QComboBox *unitsB_;

	/// Flag holding whether or not the view is basic or advanced.
	bool isBasic_;

private:
	/// Helper function that takes a "value" combo box and sets the value.
	void setValueComboBox(QComboBox *valueBox, int value);
	/// Helper function that takes a "unit" combo box and sets the value.
	void setUnitsComboBox(QComboBox *unitsBox, QString units);
};

#endif // CLSSPLITIONCHAMBERVIEW_H
