#include "SGMEnergyTrajectoryTestView.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotSeries.h"

#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/SGM/energy/SGMEnergyTrajectory.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

SGMEnergyTrajectoryTestView::SGMEnergyTrajectoryTestView(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setTheoreticalPlotData(SGMGratingSupport::LowGrating);
    setupConnections();
}

void SGMEnergyTrajectoryTestView::onCalculateButtonPushed()
{

    SGMGratingSupport::GratingTranslation correspondingGratingTranslation;

    switch(gratingTranslationComboBox_->currentIndex()) {
    case 0:
        correspondingGratingTranslation = SGMGratingSupport::LowGrating;
        break;
    case 1:
        correspondingGratingTranslation = SGMGratingSupport::MediumGrating;
        break;
    case 2:
        correspondingGratingTranslation = SGMGratingSupport::HighGrating;
        break;
    default:
        correspondingGratingTranslation = SGMGratingSupport::UnknownGrating;
        break;
    }

    setTheoreticalPlotData(correspondingGratingTranslation);
    setTrajectoryPlotData(correspondingGratingTranslation);
    setEnergyPlotData(correspondingGratingTranslation);
}

void SGMEnergyTrajectoryTestView::onGratingTranslationChanged(int)
{
    onCalculateButtonPushed();
}

void SGMEnergyTrajectoryTestView::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QHBoxLayout* plotAndControlsLayout = new QHBoxLayout();
    QVBoxLayout* controlSpacingLayout = new QVBoxLayout();
    QGridLayout* controlsLayout = new QGridLayout();

    QVBoxLayout* plotLayout = new QVBoxLayout();

    resultsTextEdit_ = new QTextEdit();

    mainLayout->addLayout(plotAndControlsLayout);
    controlSpacingLayout->addLayout(controlsLayout);
    plotAndControlsLayout->addLayout(controlSpacingLayout);
    plotAndControlsLayout->addLayout(plotLayout);
    mainLayout->addWidget(resultsTextEdit_);

    setLayout(mainLayout);

    startEnergySpinBox_ = new QDoubleSpinBox();
    startEnergySpinBox_->setRange(0, 5000);
    startEnergySpinBox_->setSuffix(" eV");

    controlsLayout->addWidget(new QLabel("Start Energy"), 0, 0);
    controlsLayout->addWidget(startEnergySpinBox_,0,1);


    endEnergySpinBox_ = new QDoubleSpinBox();
    endEnergySpinBox_->setRange(0, 5000);
    endEnergySpinBox_->setSuffix(" eV");

    controlsLayout->addWidget(new QLabel("End Energy"),1,0);
    controlsLayout->addWidget(endEnergySpinBox_, 1,1);


    timeSpinBox_ = new QDoubleSpinBox();
    timeSpinBox_->setRange(0, 10000);
    timeSpinBox_->setSuffix(" s");

    controlsLayout->addWidget(new QLabel("Time"), 2, 0);
    controlsLayout->addWidget(timeSpinBox_, 2,1);


    gratingTranslationComboBox_ = new QComboBox();
    gratingTranslationComboBox_->addItem("Low");
    gratingTranslationComboBox_->addItem("Medium");
    gratingTranslationComboBox_->addItem("High");

    controlsLayout->addWidget(new QLabel("Grating Translation"), 3, 0, 1, 2);
    controlsLayout->addWidget(gratingTranslationComboBox_, 4, 0, 1, 2);

    calculateButton_ = new QPushButton("Calculate");
    controlsLayout->addWidget(calculateButton_, 5, 0);
    controlSpacingLayout->addStretch();
    // Setup plot widgets

    plotComboBox_ = new QComboBox();
    plotComboBox_->addItem("Grating Angle");
    plotComboBox_->addItem("Undulator Position");
    plotComboBox_->addItem("Exit Slit Position");
    plotComboBox_->addItem("Energy Produced on Trajectory");

    plotLayout->addWidget(plotComboBox_);
    QPen theoreticalLinePen(QBrush(QColor(Qt::red)), 1);
    QPen trajectoryLinePen(QBrush(QColor(Qt::green)), 1);
    plotStackWidget_ = new QStackedWidget();
    plotLayout->addWidget(plotStackWidget_);

    // Grating Angle Plot
    MPlotWidget* gratingAnglePlotWidget = new MPlotWidget();
    plotStackWidget_->addWidget(gratingAnglePlotWidget);
    gratingAnglePlotWidget->enableAntiAliasing(true);

    MPlot* gratingAnglePlot = new MPlot(QRect(0,0,320, 480));
    gratingAnglePlotWidget->setPlot(gratingAnglePlot);

    gratingAnglePlot->axisBottom()->setAxisName("Target Energy (eV)");
    gratingAnglePlot->axisLeft()->setAxisName("Position (Encoder Steps)");

    gratingAnglePlot->axisScaleLeft()->setAutoScaleEnabled(true);
    gratingAnglePlot->axisScaleBottom()->setAutoScaleEnabled(true);

    gratingAngleTheoreticalData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* gratingAngleTheoreticalSeries = new MPlotSeriesBasic();
    gratingAngleTheoreticalSeries->setModel(gratingAngleTheoreticalData_);

    gratingAngleTrajectoryData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* gratingAngleTrajectorySeries = new MPlotSeriesBasic();
    gratingAngleTrajectorySeries->setModel(gratingAngleTrajectoryData_);

    gratingAnglePlot->addItem(gratingAngleTheoreticalSeries);
    gratingAnglePlot->addItem(gratingAngleTrajectorySeries);

    gratingAngleTheoreticalSeries->setLinePen(theoreticalLinePen);
    gratingAngleTheoreticalSeries->setMarker(MPlotMarkerShape::None);
    gratingAngleTheoreticalSeries->setDescription("Theoretical grating angle");

    gratingAngleTrajectorySeries->setLinePen(trajectoryLinePen);
    gratingAngleTrajectorySeries->setMarker(MPlotMarkerShape::None);
    gratingAngleTrajectorySeries->setDescription("Trajectory grating angle");

    // Undulator Position
    MPlotWidget* undulatorPositionPlotWidget = new MPlotWidget();
    plotStackWidget_->addWidget(undulatorPositionPlotWidget);
    undulatorPositionPlotWidget->enableAntiAliasing(true);

    MPlot* undulatorPositionPlot = new MPlot(QRect(0,0,320, 480));
    undulatorPositionPlotWidget->setPlot(undulatorPositionPlot);

    undulatorPositionPlot->axisBottom()->setAxisName("Target Energy (eV)");
    undulatorPositionPlot->axisLeft()->setAxisName("Position (mm)");

    undulatorPositionPlot->axisScaleLeft()->setAutoScaleEnabled(true);
    undulatorPositionPlot->axisScaleBottom()->setAutoScaleEnabled(true);

    undulatorPositionTheoreticalData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* undulatorPositionTheoreticalSeries = new MPlotSeriesBasic();
    undulatorPositionTheoreticalSeries->setModel(undulatorPositionTheoreticalData_);

    undulatorPositionTrajectoryData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* undulatorPositionTrajectorySeries = new MPlotSeriesBasic();
    undulatorPositionTrajectorySeries->setModel(undulatorPositionTrajectoryData_);

    undulatorPositionPlot->addItem(undulatorPositionTheoreticalSeries);
    undulatorPositionPlot->addItem(undulatorPositionTrajectorySeries);

    undulatorPositionTheoreticalSeries->setLinePen(theoreticalLinePen);
    undulatorPositionTheoreticalSeries->setMarker(MPlotMarkerShape::None);
    undulatorPositionTheoreticalSeries->setDescription("Theoretical undulator position");

    undulatorPositionTrajectorySeries->setLinePen(trajectoryLinePen);
    undulatorPositionTrajectorySeries->setMarker(MPlotMarkerShape::None);
    undulatorPositionTrajectorySeries->setDescription("Trajectory undulator position");

    // Exit Slit Position
    MPlotWidget* exitSlitPositionPlotWidget = new MPlotWidget();
    plotStackWidget_->addWidget(exitSlitPositionPlotWidget);
    exitSlitPositionPlotWidget->enableAntiAliasing(true);

    MPlot* exitSlitPositionPlot = new MPlot(QRect(0,0,320, 480));
    exitSlitPositionPlotWidget->setPlot(exitSlitPositionPlot);

    exitSlitPositionPlot->axisBottom()->setAxisName("Target Energy (eV)");
    exitSlitPositionPlot->axisLeft()->setAxisName("Position (mm)");

    exitSlitPositionPlot->axisScaleLeft()->setAutoScaleEnabled(true);
    exitSlitPositionPlot->axisScaleBottom()->setAutoScaleEnabled(true);

    exitSlitPositionTheoreticalData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* exitSlitPositionTheoreticalSeries = new MPlotSeriesBasic();
    exitSlitPositionTheoreticalSeries->setModel(exitSlitPositionTheoreticalData_);

    exitSlitPositionTrajectoryData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* exitSlitPositionTrajectorySeries = new MPlotSeriesBasic();
    exitSlitPositionTrajectorySeries->setModel(exitSlitPositionTrajectoryData_);

    exitSlitPositionPlot->addItem(exitSlitPositionTheoreticalSeries);
    exitSlitPositionPlot->addItem(exitSlitPositionTrajectorySeries);

    exitSlitPositionTheoreticalSeries->setLinePen(theoreticalLinePen);
    exitSlitPositionTheoreticalSeries->setMarker(MPlotMarkerShape::None);
    exitSlitPositionTheoreticalSeries->setDescription("Theoretical exit slit position");

    exitSlitPositionTrajectorySeries->setLinePen(trajectoryLinePen);
    exitSlitPositionTrajectorySeries->setMarker(MPlotMarkerShape::None);
    exitSlitPositionTrajectorySeries->setDescription("Trajectory exit slit position");

    // Exit Slit Position
    MPlotWidget* energyVsTimePlotWidget = new MPlotWidget();
    plotStackWidget_->addWidget(energyVsTimePlotWidget);
    energyVsTimePlotWidget->enableAntiAliasing(true);

    MPlot* energyVsTimePlot = new MPlot(QRect(0,0,320, 480));
    energyVsTimePlotWidget->setPlot(energyVsTimePlot);

    energyVsTimePlot->axisBottom()->setAxisName("Delta T");
    energyVsTimePlot->axisLeft()->setAxisName("Resultant Energy");

    energyVsTimePlot->axisScaleLeft()->setAutoScaleEnabled(true);
    energyVsTimePlot->axisScaleBottom()->setAutoScaleEnabled(true);

    energyVsTimeData_ = new MPlotVectorSeriesData();
    MPlotSeriesBasic* energyVsTimeSeries = new MPlotSeriesBasic();
    energyVsTimeSeries->setModel(energyVsTimeData_);

    energyVsTimePlot->addItem(energyVsTimeSeries);

    energyVsTimeSeries->setLinePen(theoreticalLinePen);
    energyVsTimeSeries->setMarker(MPlotMarkerShape::None);
    energyVsTimeSeries->setDescription("Energy produced by constant velocity grating angle");
}

void SGMEnergyTrajectoryTestView::setTheoreticalPlotData(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    if(gratingTranslation != SGMGratingSupport::UnknownGrating) {
        double startValue;
        double endValue;

        switch(gratingTranslation) {
        case SGMGratingSupport::LowGrating:
            startValue = 200;
            endValue = 800;
            break;
        case SGMGratingSupport::MediumGrating:
            startValue = 400;
            endValue = 1400;
            break;
        case SGMGratingSupport::HighGrating:
            startValue = 600;
            endValue = 2500;
            break;
        default:
            startValue = 0;
            endValue = 0;
        }

        double increment = 1;

        int numberOfItems = int((endValue - startValue) / increment);
        SGMEnergyPosition energyPosition(200, gratingTranslation);

        QVector<qreal> xValues = QVector<qreal>(numberOfItems);
        QVector<qreal> gratingAngleYValues = QVector<qreal>(numberOfItems);
        QVector<qreal> undulatorPositionYValues = QVector<qreal>(numberOfItems);
        QVector<qreal> exitSlitPositionYValues = QVector<qreal>(numberOfItems);

        double currentEnergy = startValue;
        for(int iDataPoint = 0; iDataPoint < numberOfItems; ++iDataPoint) {

			energyPosition.requestEnergy(currentEnergy);

			xValues[iDataPoint] = energyPosition.resultantEnergy();
			gratingAngleYValues[iDataPoint] = energyPosition.gratingAngle();
			undulatorPositionYValues[iDataPoint] = energyPosition.undulatorPosition();
			exitSlitPositionYValues[iDataPoint] = energyPosition.exitSlitPosition();

            currentEnergy += increment;
        }

        gratingAngleTheoreticalData_->setValues(xValues, gratingAngleYValues);
        undulatorPositionTheoreticalData_->setValues(xValues, undulatorPositionYValues);
        exitSlitPositionTheoreticalData_->setValues(xValues, exitSlitPositionYValues);
    }
}

void SGMEnergyTrajectoryTestView::setEnergyPlotData(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    if(qAbs(startEnergySpinBox_->value()) > 0.001 &&
            qAbs(endEnergySpinBox_->value()) > 0.001 &&
            qAbs(timeSpinBox_->value()) > 0.001 &&
            gratingTranslation != SGMGratingSupport::UnknownGrating) {

        SGMEnergyTrajectory trajectory(startEnergySpinBox_->value(),
                                       endEnergySpinBox_->value(),
                                       timeSpinBox_->value(),
                                       gratingTranslation);

        SGMEnergyPosition currentEnergyStatus(gratingTranslation,
                                              trajectory.startGratingAngleEncoderStep(),
                                              trajectory.undulatorHarmonic(),
                                              trajectory.startUndulatorPosition(),
                                              0,
                                              trajectory.startExitSlitPosition());

        currentEnergyStatus.setAutoDetectUndulatorHarmonic(false);

        double angleEncoderValue = trajectory.startGratingAngleEncoderStep();
        double angleEncoderVelocity = trajectory.gratingAngleVelocityProfile().targetVelocity();

        double currentTime = 0;
        double endTime = trajectory.time();
        double increment = 1;
        int numberOfTimeSlices = int((endTime - currentTime) / increment);

        QVector<qreal> timeXValues(numberOfTimeSlices);
        QVector<qreal> energyProducedYValues(numberOfTimeSlices);

        for(int iTimeSlice = 0; iTimeSlice < numberOfTimeSlices; ++iTimeSlice) {

            timeXValues[iTimeSlice] = currentTime;
			energyProducedYValues[iTimeSlice] = currentEnergyStatus.resultantEnergy();


            angleEncoderValue += angleEncoderVelocity;
			currentEnergyStatus.setGratingAngle(angleEncoderValue);

            ++currentTime;

        }


        energyVsTimeData_->setValues(timeXValues, energyProducedYValues);
    }
}

void SGMEnergyTrajectoryTestView::setTrajectoryPlotData(SGMGratingSupport::GratingTranslation gratingTranslation)
{

    if(qAbs(startEnergySpinBox_->value()) > 0.001 &&
            qAbs(endEnergySpinBox_->value()) > 0.001 &&
            qAbs(timeSpinBox_->value()) > 0.001 &&
            gratingTranslation != SGMGratingSupport::UnknownGrating) {

        SGMEnergyTrajectory trajectory(startEnergySpinBox_->value(),
                                       endEnergySpinBox_->value(),
                                       timeSpinBox_->value(),
                                       gratingTranslation);

        if(!trajectory.hasErrors()) {

            QVector<qreal> energyXValues(2);
            QVector<qreal> gratingAngleTrajectoryYValues(2);
            QVector<qreal> undulatorPositionTrajectoryYValues(2);
            QVector<qreal> exitSlitPositionTrajectoryYValues(2);

            energyXValues[0] = trajectory.startEnergy();
            energyXValues[1] = trajectory.endEnergy();

            gratingAngleTrajectoryYValues[0] = trajectory.startGratingAngleEncoderStep();
            gratingAngleTrajectoryYValues[1] = trajectory.endGratingAngleEncoderStep();

            undulatorPositionTrajectoryYValues[0] = trajectory.startUndulatorPosition();
            undulatorPositionTrajectoryYValues[1] = trajectory.endUndulatorPosition();

            exitSlitPositionTrajectoryYValues[0] = trajectory.startExitSlitPosition();
            exitSlitPositionTrajectoryYValues[1] = trajectory.endExitSlitPosition();

            gratingAngleTrajectoryData_->setValues(energyXValues, gratingAngleTrajectoryYValues);
            undulatorPositionTrajectoryData_->setValues(energyXValues, undulatorPositionTrajectoryYValues);
            exitSlitPositionTrajectoryData_->setValues(energyXValues, exitSlitPositionTrajectoryYValues);

            resultsTextEdit_->setText(trajectory.toString());
        } else {

            resultsTextEdit_->setText(trajectory.errorMessage());
        }
    }
}

void SGMEnergyTrajectoryTestView::setupConnections()
{
    connect(plotComboBox_, SIGNAL(currentIndexChanged(int)), plotStackWidget_, SLOT(setCurrentIndex(int)));
    connect(gratingTranslationComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingTranslationChanged(int)));
    connect(calculateButton_, SIGNAL(clicked(bool)), this, SLOT(onCalculateButtonPushed()));
}

