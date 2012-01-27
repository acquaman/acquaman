#ifndef SGMENERGYPOSITIONVIEW_H
#define SGMENERGYPOSITIONVIEW_H

#include <QGroupBox>
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLabel;
class QVBoxLayout;

class SGMEnergyPositionView : public QGroupBox
{
Q_OBJECT

public:
	enum EnergyPositionViewMode{
		ViewModeAll,
		ViewModeStartOrEnd,
		ViewModeMiddle
	};

	SGMEnergyPositionView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

public slots:
	void setEnergyPosition(SGMEnergyPosition *energyPosition);

protected slots:
	void onEnergyEditingFinished();
	void onMonoEditingFinished();
	void onUndulatorEditingFinished();
	void onExitSlitEditingFinished();

	void onAlternateViewModeClicked();
	void onViewModeChanged();

protected:
	SGMEnergyPosition *energyPosition_;
	SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode_;
	SGMEnergyPositionView::EnergyPositionViewMode currentViewMode_;

	QDoubleSpinBox *energySpinBox_;
	QSpinBox *monoEncoderTargetSpinBox_;
	QSpinBox *undulatorStepSetpointSpinBox_;
	QDoubleSpinBox *exitSlitDistanceSpinBox_;
	QComboBox *sgmGratingComboBox_;

	QLabel *energyLabel_;
	QLabel *monoEncoderLabel_;
	QLabel *undulatorStartStepLabel_;
	QLabel *exitSlitDistanceLabel_;
	QLabel *sgmGratingLabel_;
	QPushButton *alternateViewModeButton_;

	QVBoxLayout *vl2_;
};

class SGMEnergyPositionWBeamlineView : public SGMEnergyPositionView
{
Q_OBJECT
public:
	SGMEnergyPositionWBeamlineView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

protected slots:
	void onSetFromBeamlineButtonClicked();

protected:
	QPushButton *setFromBeamlineButton_;
};

#endif // SGMENERGYPOSITIONVIEW_H
