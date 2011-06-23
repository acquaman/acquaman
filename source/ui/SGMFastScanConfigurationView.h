#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

class QGridLayout;
class QComboBox;
class QLabel;
class QFileDialog;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QFormLayout;

class AMTopFrame;

#include <algorithm>

#include "AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

signals:

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();

protected:
	SGMFastScanConfiguration *cfg_;

	AMTopFrame *topFrame_;

	QComboBox *presetsComboBox_;
	QLabel *elementLabel_;
	QLabel *runTimeLabel_;
	QLabel *startEnergyLabel_;
	QLabel *energyMidpointLabel_;
	QLabel *endEnergyLabel_;
	QLabel *motorSettingsLabel_;
	QLabel *baseLineLabel_;
	QLabel *undulatorStartStepLabel_;
	QLabel *undulatorRelativeStepLabel_;
	QLabel *undulatorVelocityLabel_;
	QLabel *exitSlitDistanceLabel_;
	QLabel *warningsLabel_;

	QLineEdit *elementEdit_;
	QDoubleSpinBox *runTimeDSB_;
	QDoubleSpinBox *startEnergyDSB_;
	QDoubleSpinBox *energyMidpointDSB_;
	QDoubleSpinBox *endEnergyDSB_;
	QSpinBox *motorSettingsSB_;
	QSpinBox *baseLineSB_;

	QSpinBox *undulatorStartStepSB_;
	QSpinBox *undulatorRelativeStepSB_;
	QSpinBox *undulatorVelocitySB_;
	QDoubleSpinBox *exitSlitDistanceDSB_;

	QGridLayout *gl_;
	QFormLayout *fl_;
	QFormLayout *fl2_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
