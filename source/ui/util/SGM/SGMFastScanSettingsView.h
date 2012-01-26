#ifndef SGMFASTSCANSETTINGSVIEW_H
#define SGMFASTSCANSETTINGSVIEW_H

#include <QGroupBox>
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;

class SGMFastScanSettingsView : public QGroupBox
{
Q_OBJECT

public:
	SGMFastScanSettingsView(SGMFastScanSettings *fastScanSettings, QWidget *parent = 0);

public slots:
	void setFastScanSettings(SGMFastScanSettings *fastScanSettings);

protected slots:
	void onRunSecondsEditingFinished();
	void onMotorSettingsEditingFinished();
	void onBaseLineEditingFinished();
	void onUndulatorVelocityEditingFinished();

protected:
	SGMFastScanSettings *fastScanSettings_;

	QDoubleSpinBox *runSecondsSpinBox_;
	QSpinBox *motorSettingsSpinBox_;
	QSpinBox *baseLineSpinBox_;
	QSpinBox *undulatorVelocitySpinBox_;
};

#endif // SGMFASTSCANSETTINGSVIEW_H
