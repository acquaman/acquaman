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


#ifndef CLSSYNCHRONIZEDDWELLTIMEVIEW_H
#define CLSSYNCHRONIZEDDWELLTIMEVIEW_H

#include <QWidget>

#include "beamline/CLS/CLSSynchronizedDwellTime.h"

#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QComboBox>

/*!
  This widget takes in a single CLSSynchronizedDwellTimeElement and builds a view for it.  It has its own view because the elements are self-contained objects themselves.
  */
class CLSSynchronizedDwellTimeElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a CLSSynchronizedDwellTimeElement.
	virtual ~CLSSynchronizedDwellTimeElementView();
	explicit CLSSynchronizedDwellTimeElementView(CLSSynchronizedDwellTimeElement *dwellTimeElement, QWidget *parent = 0);

protected slots:
	/// Sets the checked state.
	void setEnabledState(bool enable) { if (element_->isEnabled() != enable) element_->setEnabled(enable); }
	/// Sets the time.
	void setTime();

	/// Handles changes to the status.
	void onStatusChanged(bool active) { status_->setPixmap(QIcon((active == true) ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(25, 25)); }
	/// Handles setting the units for the suffix for the time once it is accessible.  Disconnects itself once an acceptable value has been set.
	void checkTimeUnits();

protected:
	void resizeEvent(QResizeEvent *);
	/// The status icon label.
	QLabel *status_;
	/// The enabling checkbox.
	QCheckBox *enable_;
	/// The time spin box.
	QDoubleSpinBox *time_;
	/// The layout holding the widgets
	QGridLayout* elementLayout_;

	/// Pointer to the element.
	CLSSynchronizedDwellTimeElement *element_;
};

/*!
  This widget takes in a CLSSynchronizedDwellTime object and builds a view for it.  The current look is based on the current EDM version found on VESPERS.
  */
class CLSSynchronizedDwellTimeView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes a CLSSynchronizedDwellTime object.
	virtual ~CLSSynchronizedDwellTimeView();
	explicit CLSSynchronizedDwellTimeView(CLSSynchronizedDwellTime *dwellTime, QWidget *parent = 0);

signals:

public slots:
	void setAdvancedViewVisible(bool advancedViewVisible);

protected slots:
	/// Builds a popup menu for switching view modes.
	void onCustomContextMenuRequested(QPoint pos);
	/// Sets the value of the Dwell Time spinbox
	void onDwellTimeChanged(double);
	/// Sets the widgets status depending the the scanning status
	void onScanningChanged(bool);
	/// Ensures that the widgets get updated if the Dwell Time is connected after the views constructor
	void onDwellTimeConnected(bool);
	/// Sets the selected index in the mode combo box
	void onModeChanged(CLSSynchronizedDwellTime::Mode);
	/// Sets the dwell time
	void onDwellTimeSpinBoxValueChanged();
	/// Starts a scan running
	void onStartButtonClicked();
	/// Stops the running scan
	void onStopButtonClicked();
	/// Changes the mode of the scan
	void onModeComboBoxCurrentIndexChanged(int);
	/// Handles changing the status label to on or off.
	void onStatusChanged(bool status);

protected:


	/// Flag handling the visibility.
	bool advancedView_;
	QLabel* statusLabel_;
	QPushButton* startButton_;
	QPushButton* stopButton_;
	QComboBox* modeComboBox_;
	QSpinBox* dwellTimeSpinBox_;
	QLabel* dwellTimeLabel_;
	/// List of the individual element views.
	QList<CLSSynchronizedDwellTimeElementView *> elViews_;

	/// Pointer to the synchronized dwell time object.
	CLSSynchronizedDwellTime *dwellTime_;
};

#endif // CLSSYNCHRONIZEDDWELLTIMEVIEW_H
