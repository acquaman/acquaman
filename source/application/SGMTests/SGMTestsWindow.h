#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class SGMEnergyTrajectoryTestView;
class SGMEnergyPositionTestView;
class SGMEnergyControlTestView;
class SGMTestsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SGMTestsWindow(QWidget *parent = 0);

signals:

public slots:
protected slots:
    void onShowEnergyViewClicked();

    void onShowTrajectoryViewClicked();

    void onShowEnergyControlViewClicked();
protected:

    void produceTestUndulatorValues();

    void setupUi();
    QTextEdit* outputTextEdit_;
    QPushButton* showEnergyViewButton_;
    QPushButton* showTrajectoryViewButton_;
    QPushButton* showEnergyControlViewButton_;

    SGMEnergyTrajectoryTestView* energyTrajectoryTestView_;
    SGMEnergyPositionTestView* energyTestView_;
    SGMEnergyControlTestView* energyControlTestView_;
};

#endif // SGMTESTSWINDOW_H
