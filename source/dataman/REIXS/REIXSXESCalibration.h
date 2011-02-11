#ifndef REIXSXESCALIBRATION_H
#define REIXSXESCALIBRATION_H

#include "dataman/AMDbObject.h"
#include "dataman/AMControlInfoList.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

class REIXSXESCalibration : public AMDbObject
{
	Q_OBJECT
public:
	explicit REIXSXESCalibration(QObject *parent = 0);

	AMControlInfoList computeSpectrometerPosition(REIXSXESScanConfiguration* scanConfiguration) const {
		return AMControlInfoList();
	}

	QPair<double,double> evRangeForGrating(int gratingNumber) const {
		return QPair<double,double>(10, 1400);
	}

	QStringList gratingNames() const { return QStringList(); }

signals:

public slots:

};

#endif // REIXSXESCALIBRATION_H
