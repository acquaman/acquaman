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
protected:

	void setupUi();

	SGMHexapod* hexapod_;
	SGMHexapodTrajectoryView* hexapodTrajectoryView_;
	SGMHexapodSystemRotationView* hexapodSystemRotationView_;

};

#endif // SGMHEXAPODVIEW_H
