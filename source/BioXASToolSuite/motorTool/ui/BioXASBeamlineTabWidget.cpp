#include "BioXASBeamlineTabWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QImage>
#include <QPushButton>

#include "ui/beamline/AMExtendedControlEditor.h"

#include "../BioXASMAXvMotor.h"
#include "MAXvMotorControlEditor.h"

BioXASBeamlineTabWidget::BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASBeamLineID beamlineId, QWidget *parent) :
	QWidget(parent)
{
	resize(1190, 1600);

	beamlineId_ = beamlineId;
	bioXASMotorModel_ = BioXASBeamlineDataModel::instance();

	setupUiLayout();
}

BioXASBeamlineTabWidget::~BioXASBeamlineTabWidget()
{

}

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

	QString beamlineImageResource = getBeamlineImageName(beamlineId_);
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

	QGroupBox *motorGroupBox = setupMotorGroupLayout("Carbon Filter Farm Motors", BioXASBeamlineDef::FilterMotor, 170);
	firstRowPVLayout->addWidget(motorGroupBox);

	motorGroupBox = setupMotorGroupLayout(beamlineName + " Mask Motors", BioXASBeamlineDef::MaskMotor, 170);
	firstRowPVLayout->addWidget(motorGroupBox);

	motorGroupBox = setupMotorGroupLayout(beamlineName + " M1 Motors", BioXASBeamlineDef::M1Motor);
	secondRowPVLayout->addWidget(motorGroupBox);

	motorGroupBox = setupMotorGroupLayout(beamlineName + " Mono Motors", BioXASBeamlineDef::MonoMotor);
	secondRowPVLayout->addWidget(motorGroupBox);

	motorGroupBox = setupMotorGroupLayout(beamlineName + " M2 Motors", BioXASBeamlineDef::M2Motor);
	secondRowPVLayout->addWidget(motorGroupBox);
}

QString BioXASBeamlineTabWidget::getBeamlineImageName(BioXASBeamlineDef::BioXASBeamLineID beamlineId) {
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

QGroupBox * BioXASBeamlineTabWidget::setupMotorGroupLayout(QString groupBoxTitle, BioXASBeamlineDef::BioXASMotorType motorType, int expectedHeight, int expectedWidth)
{
	QVBoxLayout *motorsVBox = new QVBoxLayout();
	setupMotorsLayout(motorType, motorsVBox);

	QGroupBox *pvGroupBox = new QGroupBox(groupBoxTitle);
	pvGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	pvGroupBox->setFixedSize(expectedWidth, expectedHeight);
	pvGroupBox->setLayout(motorsVBox);

	return pvGroupBox;
}

void BioXASBeamlineTabWidget::setupMotorsLayout(BioXASBeamlineDef::BioXASMotorType motorType, QVBoxLayout *pvLayoutBox)
{
	QList<BioXASMAXvMotor *> matchedMotors = bioXASMotorModel_->getBeamlineMotorsByMotorType(beamlineId_, motorType);
	for (int i = 0; i < matchedMotors.size(); i++) {
		MAXvMotorControlEditor *motorEditor = new MAXvMotorControlEditor(matchedMotors[i], matchedMotors[i]->statusPVControl());
		pvLayoutBox->addWidget(motorEditor);
	}
}
