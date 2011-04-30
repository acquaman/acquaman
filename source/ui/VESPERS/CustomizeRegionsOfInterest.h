#ifndef CUSTOMIZEREGIONSOFINTEREST_H
#define CUSTOMIZEREGIONSOFINTEREST_H

#include <QWidget>
#include <QDoubleSpinBox>

#include "beamline/AMROI.h"

class RegionOfInterestView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMROI and gives access to customizing the low and high values.  Everything is handled in the constructor.
	explicit RegionOfInterestView(AMROI *roi, QWidget *parent = 0);

private slots:
	/// Casts an int to a double for the spin box.
	void lowUpdate(int val) { low_->setValue((double)val); }
	/// Casts an into to a double for the high spin box.
	void highUpdate(int val) { high_->setValue((double)val); }

private:
	// The spin boxes.
	QDoubleSpinBox *low_;
	QDoubleSpinBox *high_;
};

class CustomizeRegionsOfInterest : public QWidget
{
	Q_OBJECT
public:
	explicit CustomizeRegionsOfInterest(QWidget *parent = 0);

signals:

public slots:

};

#endif // CUSTOMIZEREGIONSOFINTEREST_H
