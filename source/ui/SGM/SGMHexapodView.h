#ifndef SGMHEXAPODVIEW_H
#define SGMHEXAPODVIEW_H

#include <QWidget>
#include <QPushButton>
class SGMHexapodSystemRotationView;
class SGMHexapodTrajectoryView;
class AMMotorGroupObjectView;
class SGMHexapod;
class SGMHexapodView : public QWidget
{
    Q_OBJECT
public:
	explicit SGMHexapodView(SGMHexapod* hexapod, QWidget *parent = 0);

signals:

public slots:

protected slots:
	// Test
	void onTestMoveButtonPushed();
	// end test
protected:

	void setupUi();

	SGMHexapod* hexapod_;
	SGMHexapodTrajectoryView* hexapodTrajectoryView_;
	SGMHexapodSystemRotationView* hexapodSystemRotationView_;

	// Test
	QPushButton* testCoordinatedMoveButton_;
	// End Test
};

#endif // SGMHEXAPODVIEW_H
