#ifndef MOTORTOOLMAINSCREEN_H
#define MOTORTOOLMAINSCREEN_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCloseEvent>

#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/AMBeamline.h"

class MotorToolMainScreen : public QWidget
{
    Q_OBJECT

public:
	explicit MotorToolMainScreen(BioXASBeamlineDef::BioXASBeamLineID beamlineId, QWidget *parent = 0);
    ~MotorToolMainScreen();

signals:
	void closed();

protected:
	void closeEvent(QCloseEvent *event);

	/*
	 the beamline Tab widget:
	 |--VBoxLayout---------------------------------------------|
	 |  HBoxLayout        |                |                   |
	 |   QGroupBox        |                |  QGroupBox        |
	 |    VBoxLayout      |                |   VBoxLayout      |
	 |     (Filter Motors)|                |     (Mask Motors) |
	 |                    |                |                   |
	 |---------------------------------------------------------|
	 |  HBoxLayout                                             |
	 |                                                         |
	 |     beamline image                                      |
	 |---------------------------------------------------------|
	 |  HBoxLayout        |                 |                  |
	 |   QGroupBox        |                 |  QGroupBox       |
	 |    VBoxLayout      |                 |   VBoxLayout     |
	 |     (M1 Motors)    |   (Mono Motors) |     (M2 Motors)  |
	 |                    |                 |                  |
	 |---------------------------------------------------------|
	*/
	void setupUiLayout();

private:
	BioXASBeamlineDef::BioXASBeamLineID beamlineId_;

	QString getBeamlineName();
	QString getBeamlineImageName(BioXASBeamlineDef::BioXASBeamLineID beamlineId);
	QGroupBox * setupMotorGroupLayout(QString groupBoxTitle, BioXASBeamlineDef::BioXASMotorType motorType, int expectedHeight=600, int expectedWidth=320);
	void setupMotorsLayout(BioXASBeamlineDef::BioXASMotorType motorType, QVBoxLayout *pvLayoutBox);

};

#endif // MOTORTOOLMAINSCREEN_H
