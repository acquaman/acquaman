#ifndef BIOXASBEAMLINETABWIDGET_H
#define BIOXASBEAMLINETABWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>

#include "../BioXASBeamline.h"
#include "../BioXASBeamlineDataModel.h"

class BioXASBeamlineTabWidget : public QWidget
{
    Q_OBJECT
public:
	explicit BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASBeamLineID beamlineId, QWidget *parent = 0);
	~BioXASBeamlineTabWidget();

protected:
	BioXASBeamlineDef::BioXASBeamLineID beamlineId_;

	BioXASBeamlineDataModel *bioXASMotorModel_;

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
	QString getBeamlineImageName(BioXASBeamlineDef::BioXASBeamLineID beamlineId);
	QGroupBox * setupMotorGroupLayout(QString groupBoxTitle, BioXASBeamlineDef::BioXASMotorType motorType, int expectedHeight=600, int expectedWidth=320);
	void setupMotorsLayout(BioXASBeamlineDef::BioXASMotorType motorType, QVBoxLayout *pvLayoutBox);
};

#endif // BIOXASBEAMLINETABWIDGET_H
