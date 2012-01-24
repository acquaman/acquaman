#ifndef SGMENERGYPOSITIONVIEW_H
#define SGMENERGYPOSITIONVIEW_H

#include <QGroupBox>
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;

class SGMEnergyPositionView : public QGroupBox
{
Q_OBJECT

public:
	SGMEnergyPositionView(SGMEnergyPosition *energyPosition, QWidget *parent = 0);

protected:
	SGMEnergyPosition *energyPosition_;

	QDoubleSpinBox *energySpinBox_;
	QSpinBox *monoEncoderTargetSpinBox_;
	QSpinBox *undulatorStepSetpointSpinBox_;
	QDoubleSpinBox *exitSlitDistanceSpinBox_;
	QComboBox *sgmGratingComboBox_;
};

#endif // SGMENERGYPOSITIONVIEW_H
