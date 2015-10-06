#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class SGMEnergyTrajectoryTestView;
class SGMEnergyPositionTestView;
class SGMTestsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SGMTestsWindow(QWidget *parent = 0);

signals:

public slots:
protected slots:
    void onShowMonoViewClicked();

    void onShowTrajectoryViewClicked();
protected:

    void setupUi();
    QTextEdit* outputTextEdit_;
    QPushButton* showMonoViewButton_;
    QPushButton* showTrajectoryViewButton_;

    SGMEnergyTrajectoryTestView* energyTestView_;
    SGMEnergyPositionTestView* monoInfoTestView_;
};

#endif // SGMTESTSWINDOW_H
