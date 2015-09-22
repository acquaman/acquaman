#ifndef SGMENERGYTRAJECTORYTESTVIEW_H
#define SGMENERGYTRAJECTORYTESTVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QStackedWidget>

#include "beamline/SGM/monochromator/SGMGratingSupport.h"

class MPlotWidget;
class MPlotVectorSeriesData;
class SGMEnergyTrajectoryTestView : public QWidget
{
    Q_OBJECT
public:
    explicit SGMEnergyTrajectoryTestView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    void onCalculateButtonPushed();
    void onGratingTranslationChanged(int);
protected:
    void setupUi();
    void setTheoreticalPlotData(SGMGratingSupport::GratingTranslation gratingTranslation);
    void setTrajectoryPlotData(SGMGratingSupport::GratingTranslation gratingTranslation);
    void setupConnections();

    MPlotVectorSeriesData* gratingAngleTheoreticalData_;
    MPlotVectorSeriesData* undulatorPositionTheoreticalData_;
    MPlotVectorSeriesData* exitSlitPositionTheoreticalData_;

    MPlotVectorSeriesData* gratingAngleTrajectoryData_;
    MPlotVectorSeriesData* undulatorPositionTrajectoryData_;
    MPlotVectorSeriesData* exitSlitPositionTrajectoryData_;

    MPlotWidget* plotWidget_;

    QDoubleSpinBox* startEnergySpinBox_;
    QDoubleSpinBox* endEnergySpinBox_;
    QDoubleSpinBox* timeSpinBox_;
    QComboBox* gratingTranslationComboBox_;
    QPushButton* calculateButton_;
    QStackedWidget* plotStackWidget_;

    QTextEdit* resultsTextEdit_;

    QComboBox* plotComboBox_;
};

#endif // SGMENERGYTRAJECTORYTESTVIEW_H
