#ifndef SGMENERGYCONTROLTESTVIEW_H
#define SGMENERGYCONTROLTESTVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/energy/SGMUndulatorSupport.h"
#include "beamline/SGM/energy/SGMGratingSupport.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/SGM/SGMBeamCoordinatorControl.h"
class SGMEnergyCoordinatorControl;
/*!
  * A view class for visualizing interactions with the sgm energy control
  */
class SGMEnergyControlTestView : public QWidget
{
	Q_OBJECT
public:
	/*!
	  * Creates an instance of an sgm energy control test view, which will
	  * visualize the energy controls of the sgm beamline.
	  */
	explicit SGMEnergyControlTestView(QWidget *parent = 0);

signals:

public slots:
protected slots:
	/// Initializes the child widgets when the energy control is first connected.
	void initializeOnConnection();

	// Slots which handle changes to the widgets, pushing values on the control.
	void onUndulatorHarmonicSelectionChanged(int index);
	void onUndulatorOffsetValueChanged(double value);
	void onUndulatorTrackingCheckBoxChanged(bool isChecked);
	void onGratingOptimizationSelectionChanged(int index);
	void onGratingTranslationSelectionChanged(int index);
	void onExitSlitTrackingCheckBoxChanged(bool isChecked);

	void onStartTrajectoryButtonPushed();
	// Slots which handle changes in the control, updating the widgets
	void onControlGratingOptimizationChanged(SGMGratingSupport::GratingTranslationOptimizationMode optimizationMode);
	void onControlGratingTranslationChanged(double value);
protected:

	// Setup helper functions
	/// Helper funciton to initialize the child widgets and perform layout
	void setupUi();
	/// Helper function which initializes the child widget values.
	void setupData();
	/// Helper function which connects the child widgets to the energy control.
	void setupConnections();

	SGMEnergyCoordinatorControl* energyCoordinatorControl_;
	SGMBeamCoordinatorControl* beamCoordinatorControl_;

	AMExtendedControlEditor* energyControlEditor_;
	AMExtendedControlEditor* beamControlEditor_;
	QComboBox* undulatorHarmonic_;
	QDoubleSpinBox* undulatorOffset_;
	QCheckBox* undulatorTrackingCheckBox_;
	QComboBox* gratingOptimizationComboBox_;
	QComboBox* gratingTranslationComboBox_;
	QCheckBox* exitSlitTrackingCheckBox_;

	AMExtendedControlEditor* gratingAngleValueControlEditor_;
	AMExtendedControlEditor* gratingTranslationControlEditor_;
	AMExtendedControlEditor* undulatorValueControlEditor_;
	AMExtendedControlEditor* exitSlitValueControlEditor_;

	QDoubleSpinBox* startEnergySpinBox_;
	QDoubleSpinBox* endEnergySpinBox_;
	QDoubleSpinBox* timeTakenSpinBox_;
	QPushButton* startTrajectoryButton_;
	bool isInitialized_;
};

#endif // SGMENERGYCONTROLTESTVIEW_H
