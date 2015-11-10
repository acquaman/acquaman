#ifndef SGMENERGYTRAJECTORYVIEW_H
#define SGMENERGYTRAJECTORYVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>

class SGMEnergyControlSet;
class SGMEnergyTrajectoryView : public QWidget
{
	Q_OBJECT
public:
	explicit SGMEnergyTrajectoryView(SGMEnergyControlSet* controlSet,
	                                 QWidget *parent = 0);

signals:

public slots:
protected slots:
	void onStartPushButtonClicked();

	void onControlsConnected(bool isConnected);
protected:
	void setupUi();

	void setupData();

	void setupConnections();
	SGMEnergyControlSet* controlSet_;

	QDoubleSpinBox* startpointSpinBox_;
	QDoubleSpinBox* endpointSpinBox_;
	QDoubleSpinBox* timeSpinBox_;
	QPushButton* startButton_;

	bool connectedOnce_;
};

#endif // SGMENERGYTRAJECTORYVIEW_H
