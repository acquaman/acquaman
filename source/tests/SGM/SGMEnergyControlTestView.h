#ifndef SGMENERGYCONTROLTESTVIEW_H
#define SGMENERGYCONTROLTESTVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/energy/SGMUndulatorSupport.h"
#include "beamline/SGM/energy/SGMGratingSupport.h"
#include "beamline/SGM/energy/SGMEnergyPosition.h"
class SGMEnergyControl;
class SGMEnergyControlTestView : public QWidget
{
    Q_OBJECT
public:
    explicit SGMEnergyControlTestView(QWidget *parent = 0);

signals:

public slots:
protected slots:
    void initializeOnConnection();

    void onUndulatorHarmonicSelectionChanged(int index);
    void onUndulatorOffsetValueChanged(double value);
    void onUndulatorTrackingCheckBoxChanged(bool isChecked);
    void onGratingOptimizationSelectionChanged(int index);
    void onExitSlitTrackingCheckBoxChanged(bool isChecked);

    void onControlGratingOptimizationChanged(SGMEnergyPosition::GratingTranslationOptimizationMode optimizationMode);

protected:
    void setupUi();
    void setupData();
    void setupConnections();

    SGMEnergyControl* energyControl_;

    AMExtendedControlEditor* energyControlEditor_;
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
    bool isInitialized_;
};

#endif // SGMENERGYCONTROLTESTVIEW_H
