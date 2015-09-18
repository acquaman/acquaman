#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATIONVIEW_H

#include <QWidget>

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"
#include "ui/dataman/AMScanView.h"

#include "acquaman/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration.h"


class BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView(BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *config, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView();
	/// This function must return a pointer to the scan configuration, that is being configured within your view.
	/*! Any functions that use this will make a copy of the configuration immediately, so you don't need to worry about making changes to the pointed-to object.*/
	virtual const AMScanConfiguration* configuration() const { return configuration_; }

signals:

public slots:

protected:
	/// The scan configuration.
	BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *configuration_;
	/// The scan set model.
	AMScanSetModel *scanSetModel_;
	/// The scan view.
	AMScanView *scanView_;
	/// The scan configuration editor.
	BioXASXASScanConfigurationView *configView_;
	/// The scan configuration editor holder.
	AMScanConfigurationViewHolder3 *configViewHolder_;

};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONSCANCONFIGURATIONVIEW_H
