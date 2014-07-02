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


#ifndef AMCONTROLSETVIEW_H
#define AMCONTROLSETVIEW_H

#include "beamline/AMControlSet.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QAbstractSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QGraphicsItem>

#include <QTableView>
#include <QPen>
#include <QBrush>

/// An AMControlSetView is a means of generating a default view for an AMControlSet
/*!
  Inheriting from QGroupBox, the AMControlSetView displays the contents of an AMControlSet in a group box.
  Simplying calling the constructor while passing an AMControlSet allows the AMControlSetView to:
  - set the title of the group box from the name of the AMControlSet; and,
  - generate a label for each AMControl in the AMControlSet from the objectName() function; and,
  - generate a spin box (integer for enums and double for all others) for each AMControl in the AMControlSet.
  */
class AMControlSetView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor
	/*! \param viewSet Pointer to the AMControlSet to view.
	  \param parent Pointer to QWidget to act as parent.
	  */
 	virtual ~AMControlSetView();
	explicit AMControlSetView(AMControlSet *viewSet, bool configureOnly = false, QWidget *parent = 0);

	AMExtendedControlEditor* boxByName(const QString &name){
		return controlBoxes_.at(viewSet_->indexOfKey(name));
	}

	AMExtendedControlEditor* boxAt(int row){
		return controlBoxes_.at(row);
	}

	/// Returns the current values of the control set
	AMControlInfoList currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMControlInfoList configValues();

signals:
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged(AMControlInfoList);
	/// Emitted when any configuration value changes (but ONLY when the view is configure only)
	void configValuesChanged(AMControlInfoList);

public slots:
	/* NTBA March 14, 2011 David Chevrier
	   Need something to setFromInfo, should be able to just pass along to controlSet
	   */
	void setFromInfoList(const AMControlInfoList &infoList);

	void setDisabled(bool disabled);

protected slots:
	void onControlSetValuesChanged();
	void onConfigurationValueChanged();

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMControlSet *viewSet_;
	bool configureOnly_;
	QList<AMExtendedControlEditor*> controlBoxes_;
	QHBoxLayout *hl_;
};

#endif // AMCONTROLSETVIEW_H
