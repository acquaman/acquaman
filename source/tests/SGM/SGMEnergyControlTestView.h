#ifndef SGMENERGYCONTROLTESTVIEW_H
#define SGMENERGYCONTROLTESTVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/monochromator/SGMUndulatorSupport.h"
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
    void onExitSlitTRackingCheckBoxChanged(bool isChecked);
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
    QCheckBox* exitSlitTrackingCheckBox_;

    bool isInitialized_;
};

#endif // SGMENERGYCONTROLTESTVIEW_H
