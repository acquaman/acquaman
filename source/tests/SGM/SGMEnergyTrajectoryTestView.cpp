#include "SGMEnergyTrajectoryTestView.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotSeries.h"

#include "beamline/SGM/monochromator/SGMMonochromatorInfo.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#define GRATING_ANGLE_PLOT_INDEX 0
#define UNDULATOR_POSITION_PLOT_INDEX 1
#define EXIT_SLIT_PLOT_INDEX 2
SGMEnergyTrajectoryTestView::SGMEnergyTrajectoryTestView(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setTheoreticalPlotData(SGMGratingSupport::LowGrating);
    setupConnections();
}

void SGMEnergyTrajectoryTestView::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QHBoxLayout* plotAndControlsLayout = new QHBoxLayout();

    QGridLayout* controlsLayout = new QGridLayout();

    QVBoxLayout* plotLayout = new QVBoxLayout();

    resultsTextEdit_ = new QTextEdit();

    mainLayout->addLayout(plotAndControlsLayout);
    plotAndControlsLayout->addLayout(controlsLayout);
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

    gratingTranslationComboBox_->addItem("Low");

    controlsLayout->addWidget(new QLabel("Grating Translation"), 3, 0, 1, 2);
    controlsLayout->addWidget(gratingTranslationComboBox_, 4, 0, 1, 2);

    calculateButton_ = new QPushButton("Calculate");
    controlsLayout->addWidget(calculateButton_, 5, 0);
    clearButton_ = new QPushButton("Clear");
    controlsLayout->addWidget(clearButton_, 5,1);

    plotComboBox_ = new QComboBox();
    plotComboBox_->addItem("Grating Angle");
    plotComboBox_->addItem("Undulator Position");
    plotComboBox_->addItem("Exit Slit Position");

    plotLayout->addWidget(plotComboBox_);

    // Setup plot widgets
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
}

void SGMEnergyTrajectoryTestView::setTheoreticalPlotData(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    double startValue = 200;
    double endValue = 2000;
    double increment = 50;

    int numberOfItems = int((endValue - startValue) / increment);
    SGMMonochromatorInfo monoInfo(200, gratingTranslation);

    QVector<qreal> xValues = QVector<qreal>(numberOfItems);
    QVector<qreal> gratingAngleYValues = QVector<qreal>(numberOfItems);
    QVector<qreal> undulatorPositionYValues = QVector<qreal>(numberOfItems);
    QVector<qreal> exitSlitPositionYValues = QVector<qreal>(numberOfItems);

    xValues.clear();
    gratingAngleYValues.clear();
    undulatorPositionYValues.clear();
    exitSlitPositionYValues.clear();

    for(double energy = startValue; energy <= endValue; energy+=increment) {

        monoInfo.requestEnergy(energy, gratingTranslation);
        xValues.append(monoInfo.resultantEnergy());
        gratingAngleYValues.append(monoInfo.gratingAngle());
        undulatorPositionYValues.append(monoInfo.undulatorPosition());
        exitSlitPositionYValues.append(monoInfo.exitSlitPosition());
    }

    gratingAngleTheoreticalData_->setValues(xValues, gratingAngleYValues);
    undulatorPositionTheoreticalData_->setValues(xValues, undulatorPositionYValues);
    exitSlitPositionTheoreticalData_->setValues(xValues, exitSlitPositionYValues);
}

void SGMEnergyTrajectoryTestView::setTrajectoryPlotData(SGMGratingSupport::GratingTranslation gratingTranslation)
{

}

void SGMEnergyTrajectoryTestView::setupConnections()
{
    connect(plotComboBox_, SIGNAL(currentIndexChanged(int)), plotStackWidget_, SLOT(setCurrentIndex(int)));
}

