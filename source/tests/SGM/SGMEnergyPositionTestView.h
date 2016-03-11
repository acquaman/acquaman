#ifndef SGMENERGYPOSITIONTESTVIEW_H
#define SGMENERGYPOSITIONTESTVIEW_H

#include <QWidget>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "beamline/SGM/energy/SGMGratingSupport.h"
#include "beamline/SGM/energy/SGMUndulatorSupport.h"
class SGMEnergyPosition;
/*!
 * A widget class designed to visualize the data within an SGMMonochromatorInfo
 * for testing purposes.
 */
class SGMEnergyPositionTestView : public QWidget
{
	Q_OBJECT
public:
	/*!
	 * Creates an instance of an mono info test view which will visualize the
	 * provided Energy Position.
	 */
	explicit SGMEnergyPositionTestView(SGMEnergyPosition* energyPosition, QWidget *parent = 0);

signals:

public slots:

protected slots:

	/*
	 * Signals from the energy position to update the view
	 */

	void onEnergyPositionGratingAngleChanged(double gratingAngleSteps);
	void onEnergyPositionGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);
	void onEnergyPositionUndulatorTrackingChanged(bool isTracking);
	void onEnergyUndulatorPositionChanged(double undulatorPosition);
	void onEnergyPositionUndulatorOffsetChanged(double undulatorOffset);
	void onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);
	void onEnergyPositionExitSlitTrackingChanged(bool isTracking);
	void onEnergyPositionExitSlitPositionChanged(double exitSlitPosition);
	void onEnergyPositionEnergyChanged(double energy);
	void onEnergyPositionWarningCountChanged(int warningCount);
	void onEnergyPositionErrorCountChanged(int errorCount);


	/*
	 * Signals from the view widgets to update the mono
	 */

	void onGratingAngleSpinBoxChanged();
	void onGratingSelectionModeComboBoxChanged(int index);
	void onGratingTranslationComboBoxChanged(int index);
	void onUndulatorTrackingCheckBoxChanged(bool value);
	void onUndulatorPositionSpinBoxChanged();
	void onUndulatorOffsetSpinBoxChanged();
	void onAutoDetectUndulatorHarmonicCheckBoxChanged(bool value);
	void onUndulatorHarmonicComboBoxChanged(int index);
	void onExitSlitTrackingCheckBoxChanged(bool value);
	void onExitSlitPositionSpinBoxChanged();
	void onEnergySpinBoxChanged();

protected:

	/*!
	 * Initializes the layout components, and perform the initial layout.
	 */
	void setupUi();

	/*!
	 * Sets up the signal/slot connections between the widgets, this view and
	 * the mono info
	 */
	void makeConnections();

	/*!
	 * Sets up the initial data for the view.
	 */
	void setupData();

	SGMEnergyPosition* energyPosition_;

	QDoubleSpinBox* energySpinBox_;
	QComboBox* gratingSelectionModeComboBox_;
	QComboBox* gratingTranslationComboBox_;
	QDoubleSpinBox* gratingAngleSpinBox_;
	QDoubleSpinBox* undulatorPositionSpinBox_;
	QDoubleSpinBox* undulatorDetuneOffsetSpinBox_;
	QCheckBox* undulatorTrackingCheckBox_;
	QCheckBox* autoDetectUndulatorHarmonicCheckBox_;
	QComboBox* undulatorHarmonicComboBox_;
	QDoubleSpinBox* exitSlitPositionSpinBox_;
	QCheckBox* exitSlitTrackingCheckBox_;
	QTextEdit* warningsTextEdit_;
	QTextEdit* errorsTextEdit_;

};

#endif // SGMENERGYPOSITIONTESTVIEW_H
