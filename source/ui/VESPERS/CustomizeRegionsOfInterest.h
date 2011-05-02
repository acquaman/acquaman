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
	void setRoiLow(double val) { if (val != 0) roi_->setLow(val); }
	/// Sets the high value to the AMROI.
	void setRoiHigh(double val) { if (val != 0) roi_->setHigh(val); }

private slots:
	/// Checks whether the name has a name in it or not.  If the name is an empty string, the view hides itself.
	void nameUpdate(QString name);
	/// Casts an int to a double for the spin box.  If the value is zero then the ROI is not connected yet.
	void onLowUpdate(int val) { if (val != 0) low_->setValue((double)val*roi_->scale()); }
	/// Casts an into to a double for the high spin box.  If the value is zero then the ROI is not connected yet.
	void onHighUpdate(int val) { if (val != 0) high_->setValue((double)val*roi_->scale()); }

private:
	// The label.
	QLabel *name_;

	// The spin boxes.
	QDoubleSpinBox *low_;
	QDoubleSpinBox *high_;

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
