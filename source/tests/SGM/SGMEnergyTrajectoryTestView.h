#ifndef SGMENERGYTRAJECTORYTESTVIEW_H
#define SGMENERGYTRAJECTORYTESTVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QStackedWidget>

#include "beamline/SGM/energy/SGMGratingSupport.h"
#include "beamline/SGM/energy/SGMUndulatorSupport.h"

class MPlotWidget;
class MPlotVectorSeriesData;
/*!
 * A class for viewing information related to energy trajectories for the SGM beamline.
 */
class SGMEnergyTrajectoryTestView : public QWidget
{
	Q_OBJECT
public:
	/*!
	 * Creates an instance of an SGM energy trajectory view.
	 */
	explicit SGMEnergyTrajectoryTestView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/*!
	 * Calculates the provided trajectory view (if valid) and dispalys the information
	 * in the view's plots.
	 */
	void recalculate();

protected:
	/*!
	 * Helper function for initializing the ui
	 */
	void setupUi();

	/*!
	 * Helper function for updating the plot data for the theoretical range of
	 * component positions vs. energy.
	 * \param gratingTranslation ~ The grating translation used to produce the
	 * energy.
	 * \param undulatorHarmonic ~ The undulator harmonic used to produce the undulator
	 * gap value
	 */
	void setTheoreticalPlotData(SGMGratingSupport::GratingTranslation gratingTranslation,
	                            SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	 * Helper function for updating the plot data of the energy produced by the
	 * grating over a given trajectory.
	 * \param gratingTranslation ~ The grating translation used over the trajectory.
	 */
	void setEnergyPlotData(SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	 * Helper function for updating the plot data for the approximated component
	 * position relating to a given energy.
	 * \param gratingTranslation ~ The grating translation used to calculate the
	 * approximated component position.
	 */
	void setTrajectoryPlotData(SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	 * Helper function which sets up the connections between the contained widgets.
	 */
	void setupConnections();

	MPlotVectorSeriesData* gratingAngleTheoreticalData_;
	MPlotVectorSeriesData* undulatorPositionTheoreticalData_;
	MPlotVectorSeriesData* exitSlitPositionTheoreticalData_;

	MPlotVectorSeriesData* gratingAngleTrajectoryData_;
	MPlotVectorSeriesData* undulatorPositionTrajectoryData_;
	MPlotVectorSeriesData* exitSlitPositionTrajectoryData_;

	MPlotVectorSeriesData* energyVsTimeData_;

	MPlotWidget* plotWidget_;

	QDoubleSpinBox* startEnergySpinBox_;
	QDoubleSpinBox* endEnergySpinBox_;
	QDoubleSpinBox* timeSpinBox_;
	QDoubleSpinBox* undulatorOffsetSpinBox_;
	QComboBox* gratingTranslationComboBox_;
	QComboBox* undulatorHarmonicComboBox_;
	QPushButton* calculateButton_;
	QStackedWidget* plotStackWidget_;

	QTextEdit* resultsTextEdit_;

	QComboBox* plotComboBox_;
};

#endif // SGMENERGYTRAJECTORYTESTVIEW_H
