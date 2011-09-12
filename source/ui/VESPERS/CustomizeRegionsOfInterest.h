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


#ifndef CUSTOMIZEREGIONSOFINTEREST_H
#define CUSTOMIZEREGIONSOFINTEREST_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/AMROI.h"

class RegionOfInterestView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMROI and gives access to customizing the low and high values.
	explicit RegionOfInterestView(AMROI *roi, QWidget *parent = 0);

public slots:
	/// Sets the low value to the AMROI.
	void setRoiLow(double val)
	{
		disconnect(roi_, SIGNAL(lowUpdate(int)), this, SLOT(onLowUpdate(int)));
		roi_->setLow(val);
		connect(roi_, SIGNAL(lowUpdate(int)), this, SLOT(onLowUpdate(int)));
	}
	/// Sets the high value to the AMROI.
	void setRoiHigh(double val)
	{
		disconnect(roi_, SIGNAL(highUpdate(int)), this, SLOT(onHighUpdate(int)));
		roi_->setHigh(val);
		connect(roi_, SIGNAL(highUpdate(int)), this, SLOT(onHighUpdate(int)));
	}

private slots:
	/// Checks whether the name has a name in it or not.  If the name is an empty string, the view hides itself.
	void nameUpdate(QString name);
	/// Casts an int to a double for the spin box.  If the value is zero then the ROI is not connected yet.  Negative numbers are also no good.
	void onLowUpdate(int val)
	{
		if (val > 0) {

			low_->blockSignals(true);
			low_->setValue((double)val*roi_->scale());
			low_->blockSignals(false);
		}
	}
	/// Casts an into to a double for the high spin box.  If the value is zero then the ROI is not connected yet.  Negative numbers are also no good.
	void onHighUpdate(int val)
	{
		if (val > 0) {

			high_->blockSignals(true);
			high_->setValue((double)val*roi_->scale());
			high_->blockSignals(false);
		}
	}
	/// Updates the value label with the new counts value.
	void onValueUpdate(double num) { value_->setText(QString::number(num, 'g', 3)); }
	/// Handles when an ROI is initialized.
	void onRoiInialized() { nameUpdate(roi_->name()); }
	/// Handles changes in the scale.
	void onScalerChanged(double val) { low_->setSingleStep(val); high_->setSingleStep(val); }

private:
	// The label.
	QLabel *name_;

	// The spin boxes.
	QDoubleSpinBox *low_;
	QDoubleSpinBox *high_;

	// The counts value.
	QLabel *value_;

	// Pointer to the current AMROI.
	AMROI *roi_;
};

/// This class builds a list that can be used to modify the high and low values for a region of interest.
class CustomizeRegionsOfInterest : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in a list of AMROIs and displays the ones that are valid.
	explicit CustomizeRegionsOfInterest(QList<AMROI *> rois, QWidget *parent = 0);

signals:

public slots:

};

#endif // CUSTOMIZEREGIONSOFINTEREST_H
