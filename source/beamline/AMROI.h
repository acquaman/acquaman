#ifndef AMROI_H
#define AMROI_H

#include <QObject>

#include "dataman/AMROIInfo.h"

/// This class is used to set PVs based on either an AMROIInfo class or through other means (ex: a detector view).  It is meant to encapsulate the different pieces of an ROI into a single class.  For instance, the energy, the low limit, the high limit, and the name are all useful to group together.
class AMROI : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  It needs an AMControlSet to connect all the PVs to the appropriate fields.
	explicit AMROI(QObject *parent = 0);

	/// Takes an AMROIInfo and sets the AMROI to match it.
	void fromInfo(const AMROIInfo &info);
	/// Takes the current state of the AMROI and returns an AMROIInfo.
	AMROIInfo toInfo();

signals:

#warning "This class needs to be finished:  AMROI"

public slots:
	/// Sets the name of the ROI and passes it to all PV's.
	void setName(QString name);
	/// Sets the energy of the ROI and based on the current width, sets the low and high values on all PV's.
	void setEnergy(double energy);
	/// Sets the width of the ROI and based on the current energy, sets the low and high values on all PV's.
	void setWidth(double width);
	/// Explicitly changes the low value for the ROI and all the PV's.  Does not affect the energy or the width in anyway.
	void setLow(int low);
	/// Explicitly changes the high value for the ROI and all the PV's.  Does not affect the energy or the width in anyway.
	void setHigh(int high);

private:
	// Member variables.
	QString name_;
	double energy_;
	double width_;
	int low_;
	int high_;
	double value_;

};

#endif // AMROI_H
