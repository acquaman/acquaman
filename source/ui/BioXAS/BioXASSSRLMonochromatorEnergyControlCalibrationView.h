#ifndef BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "ui/dataman/AMScanView.h"

#include "beamline/BioXAS/BioXASSSRLMonochromatorEnergyControlCalibrationController.h"

class BioXASSSRLMonochromatorEnergyControlCalibrationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorEnergyControlCalibrationView(BioXASSSRLMonochromatorEnergyControlCalibrationController *controller, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorEnergyControlCalibrationView();
	/// Returns the calibration controller.
	BioXASSSRLMonochromatorEnergyControlCalibrationController* calibrationController() const { return controller_; }

signals:
	/// Notifier that the calibration controller has been changed.
	void calibrationControllerChanged(BioXASSSRLMonochromatorEnergyControlCalibrationController *newController);

public slots:
	/// Sets the calibration controller.
	void setCalibrationController(BioXASSSRLMonochromatorEnergyControlCalibrationController *newController);

protected slots:
	/// Handles updates after the energy value has changed.
	void onEnergyChanged();
	/// Handles updates after the 'apply' button is clicked.
	void onApplyButtonClicked();
	/// Handles updates after the 'cancel' button is clicked.
	void onCancelButtonClicked();

protected:
	/// The calibration controller.
	BioXASSSRLMonochromatorEnergyControlCalibrationController *controller_;

	/// The scan view.
	AMScanView *scanView_;

	/// The energy display.
	QDoubleSpinBox *energy_;
	/// The 'apply changes' button.
	QPushButton *applyButton_;
	/// The 'cancel' button.
	QPushButton *cancelButton_;
};

#endif // BIOXASSSRLMONOCHROMATORENERGYCALIBRATIONVIEW_H
