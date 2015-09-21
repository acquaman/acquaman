#ifndef SGMMONOCHROMATORINFOTESTVIEW_H
#define SGMMONOCHROMATORINFOTESTVIEW_H

#include <QWidget>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include "beamline/SGM/monochromator/SGMGratingSupport.h"
#include "beamline/SGM/monochromator/SGMUndulatorSupport.h"
class SGMMonochromatorInfo;
class SGMMonochromatorInfoTestView : public QWidget
{
    Q_OBJECT
public:
    explicit SGMMonochromatorInfoTestView(SGMMonochromatorInfo* mono, QWidget *parent = 0);

signals:

public slots:

protected slots:
    void onMonoGratingAngleChanged(double gratingAngleSteps);
    void onMonoGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);
    void onMonoUndulatorTrackingChanged(bool isTracking);
    void onMonoUndulatorPositionChanged(double undulatorPosition);
    void onMonoUndulatorOffsetChanged(double undulatorOffset);
    void onMonoUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);
    void onMonoExitSlitTrackingChanged(bool isTracking);
    void onMonoExitSlitPositionChanged(double exitSlitPosition);
    void onMonoEnergyChanged(double energy);
    void onMonoWarningCountChanged(int warningCount);
    void onMonoErrorCountChanged(int errorCount);


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

    void setupUi();

    void makeConnections();

    void setupData();

    SGMMonochromatorInfo* sgmMonochromatorInfo_;

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

#endif // SGMMONOCHROMATORINFOTESTVIEW_H
