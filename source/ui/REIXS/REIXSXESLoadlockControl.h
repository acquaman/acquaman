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


#ifndef REIXSXESLOADLOCKCONTROL_H
#define REIXSXESLOADLOCKCONTROL_H

#include <QWidget>
#include "ui/beamline/AMControlEditor.h"
#include "beamline/AMCompositeControl.h"
#include "beamline/CLS/CLSMDriveMotorControl.h"

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QRadioButton>

class QDialog;
class REIXSSampleChamber;

class REIXSXESLoadlockControl : public QWidget
{
	Q_OBJECT
public:
	REIXSXESLoadlockControl(QWidget *parent = 0);
	virtual ~REIXSXESLoadlockControl();

protected:
	/// Controls:
	/// A hierarchichal group of controls making up the sample chamber
	REIXSSampleChamber* sampleChamber_;

	/// UI components:
	QPushButton *loadlockSlot1_,*loadlockSlot2_,*loadlockSlot3_,*loadlockSlot4_,*loadlockSlotOut_;
	QLabel *motorFeedback_;

protected slots:
	// Responding to GUI events
	////////////////////////////

	void onloadlockSlot1Clicked();
	void onloadlockSlot2Clicked();
	void onloadlockSlot3Clicked();
	void onloadlockSlot4Clicked();
	void onloadlockSlotOutClicked();



protected slots:




};




/// The REIXSSampleChamber control is a container for the motor controls that make up the sample manipulator and load lock.
class REIXSSampleChamber : public AMCompositeControl {
	Q_OBJECT
public:
	virtual ~REIXSSampleChamber();
	REIXSSampleChamber(QObject* parent = 0);

	AMControl* x() { return x_; }
	AMControl* y() { return y_; }
	AMControl* z() { return z_; }
	AMControl* r() { return r_; }

	AMControl* loadLockZ() { return loadLockZ_; }
	AMControl* loadLockR() { return loadLockR_; }



protected:
	CLSMDriveMotorControl* x_, *y_, *z_, *r_, *loadLockZ_, *loadLockR_;

};





#endif // REIXSXESLOADLOCKCONTROL_H
