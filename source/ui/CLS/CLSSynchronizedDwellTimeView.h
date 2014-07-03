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
	void onStatusChanged(bool active) { status_->setPixmap(QIcon((active == true) ? ":/ON.png" : ":/OFF.png").pixmap(25, 25)); }
	/// Handles setting the units for the suffix for the time once it is accessible.  Disconnects itself once an acceptable value has been set.
	void checkTimeUnits();
	/// Handles changes to the element enabled signal.
	void onEnabledChanged(bool enabled)
	{
		if (enabled){

			enable_->setText("Enabled");
			enable_->setPalette(QPalette(Qt::green));
		}
		else{

			enable_->setText("Disabled");
			enable_->setPalette(QPalette(Qt::red));
		}
	}

protected:
	/// The status icon label.
	QLabel *status_;
	/// The enabling button.
	QPushButton *enable_;
	/// The time spin box.
	QDoubleSpinBox *time_;

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
	/// Sets the time.
	void setTime();
	/// Sets the mode for the dwell time.  If the mode is true, then set the Mode to Continuous, otherwise set it to Single Shot.
	void setMode(bool mode);
	/// Toggles the scanning state for the synchronized dwell time.
	void toggleScanning() { dwellTime_->startScanning(!dwellTime_->isScanning()); }

	/// Handles changes to the status.
	void onStatusChanged(bool active) { overallStatus_->setPixmap(QIcon((active == true) ? ":/ON.png" : ":/OFF.png").pixmap(30, 30)); }
	/// Handles changes to the mode.
	void onModeChanged(CLSSynchronizedDwellTime::Mode mode)
	{
		if (mode == CLSSynchronizedDwellTime::SingleShot){

			mode_->setText("Single Shot");
			mode_->setChecked(false);
		}
		else if (mode == CLSSynchronizedDwellTime::Continuous){

			mode_->setText("Continuous");
			mode_->setChecked(true);
		}
	}
	/// Handles changes to the scanning status.
	void onScanningChanged(bool scanning)
	{
		if (scanning){

			scanning_->setText("Scanning");
			scanning_->setPalette(QPalette(Qt::green));
		}
		else{

			scanning_->setText("Stopped");
			scanning_->setPalette(QPalette(Qt::red));
		}

		scanning_->setChecked(scanning);
	}
	/// Builds a popup menu for switching view modes.
	void onCustomContextMenuRequested(QPoint pos);

protected:
	/// The status icon label.
	QLabel *overallStatus_;
	/// The mode selection button.
	QPushButton *mode_;
	/// The scanning button.
	QPushButton *scanning_;
	/// The master time double spin box.
	QDoubleSpinBox *masterTime_;

	/// Flag handling the visibility.
	bool advancedView_;

	/// List of the individual element views.
	QList<CLSSynchronizedDwellTimeElementView *> elViews_;

	/// Pointer to the synchronized dwell time object.
	CLSSynchronizedDwellTime *dwellTime_;
};

#endif // CLSSYNCHRONIZEDDWELLTIMEVIEW_H
