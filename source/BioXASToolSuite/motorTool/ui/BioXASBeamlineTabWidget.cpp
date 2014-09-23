#include "BioXASBeamlineTabWidget.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QImage>
#include <QPushButton>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "MAXvMotorControlEditor.h"

#include "../BioXASMAXvMotor.h"

BioXASBeamlineTabWidget::BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASBeamLineID beamlineId, QWidget *parent) :
	QWidget(parent)
{
	resize(1190, 1600);

	beamlineId_ = beamlineId;
	bioXASMotorModel_ = MotorModel::instance();

	setupUiLayout();
}

/*
 the beamline Tab widget:
 |--VBoxLayout---------------------------------------------|
 |  HBoxLayout        |                |                   |
 |   VBoxLayout       |                |  VBoxLayout       |
 |     (Filter Motors)|                |     (Mask Motors) |
 |                    |                |                   |
 |---------------------------------------------------------|
 |  HBoxLayout                                             |
 |                                                         |
 |     dispaly beamline image                              |
 |---------------------------------------------------------|
 |  HBoxLayout        |                 |                  |
 |   VBoxLayout       | VBoxLayout      |  VBoxLayout      |
 |     (M1 Motors)    |   (Mono Motors) |     (M2 Motors)  |
 |                    |                 |                  |
 |---------------------------------------------------------|

*/
void BioXASBeamlineTabWidget::setupUiLayout()
{
	// the primay layout is a vertical layout
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// the second level layout
	QHBoxLayout *firstRowPVLayout = new QHBoxLayout();
	QHBoxLayout *blImageLayout = new QHBoxLayout();
	QHBoxLayout *secondRowPVLayout = new QHBoxLayout();

	mainLayout->addLayout(firstRowPVLayout);
	mainLayout->addLayout(blImageLayout);
	mainLayout->addLayout(secondRowPVLayout);

	//initialize the beamline image
	QLabel *imageLabel = new QLabel();
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	imageLabel->setFixedSize(1190, 150);

	QString beamlineImageResource = getBeamlineImage(beamlineId_);
	QImage image(beamlineImageResource);
	imageLabel->setPixmap(QPixmap::fromImage(image));

	blImageLayout->addWidget(imageLabel);

	//initialize the PV layouts
	QString beamlineName = "";
	if (beamlineId_ == BioXASBeamlineDef::BioXASMainBeamLine)
		beamlineName = "Main";
	else if (beamlineId_ == BioXASBeamlineDef::BioXASSideBeamLine)
		beamlineName = "Side";
	else if (beamlineId_ == BioXASBeamlineDef::BioXASImagingBeamLine)
		beamlineName = "Imaging";

	filterMotorVBox_ = createMotorGroupBox("Carbon Filter Farm Motors", firstRowPVLayout, 170);
	maskMotorVBox_ = createMotorGroupBox(beamlineName + " Mask Motors", firstRowPVLayout, 170);
	m1MotorVBox_ = createMotorGroupBox(beamlineName + " M1 Motors", secondRowPVLayout);
	monoMotorVBox_ = createMotorGroupBox(beamlineName + " Mono Motors", secondRowPVLayout);
	m2MotorVBox_ = createMotorGroupBox(beamlineName + " M2 Motors", secondRowPVLayout);

	setupMotors(BioXASBeamlineDef::FilterMotor, filterMotorVBox_);
	setupMotors(BioXASBeamlineDef::M1Motor, m1MotorVBox_);
	setupMotors(BioXASBeamlineDef::MaskMotor, maskMotorVBox_);
	setupMotors(BioXASBeamlineDef::MonoMotor, monoMotorVBox_);
	setupMotors(BioXASBeamlineDef::M2Motor, m2MotorVBox_);

}

void BioXASBeamlineTabWidget::setupMotors(BioXASBeamlineDef::BioXASMotorType motorType, QVBoxLayout *pvLayoutBox)
{
	QList<BioXASMAXvMotor *> matchedMotors = bioXASMotorModel_->getMotorsByCategory(beamlineId_, motorType);
	for (int i = 0; i < matchedMotors.size(); i++) {
		MAXvMotorControlEditor *motorEditor = new MAXvMotorControlEditor(matchedMotors[i], matchedMotors[i]->statusPVControl());
		pvLayoutBox->addWidget(motorEditor);
	}
}

QString BioXASBeamlineTabWidget::getBeamlineImage(BioXASBeamlineDef::BioXASBeamLineID beamlineId) {
	QString beamlineImageResource;
	switch (beamlineId) {
	case BioXASBeamlineDef::BioXASMainBeamLine:
		beamlineImageResource = ":/image/07ID_MainStation.gif";
		break;
	case BioXASBeamlineDef::BioXASSideBeamLine:
		beamlineImageResource = ":/image/07ID_SideStation.gif";
		break;
	case BioXASBeamlineDef::BioXASImagingBeamLine:
	default:
		beamlineImageResource = ":/image/07ID_Imaging.gif";
		break;
	}

	return beamlineImageResource;
}

QVBoxLayout * BioXASBeamlineTabWidget::createMotorGroupBox(QString title, QHBoxLayout * parentContainer, int expectedHeight, int expectedWidth)
{
	QGroupBox *pvGroupBox = new QGroupBox(title);
	pvGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	pvGroupBox->setFixedSize(expectedWidth, expectedHeight);


	QVBoxLayout *motorsVBox = new QVBoxLayout();
	pvGroupBox->setLayout(motorsVBox);

	parentContainer->addWidget(pvGroupBox);

	return motorsVBox;

}

