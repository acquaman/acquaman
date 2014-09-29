#include "MotorToolMainScreen.h"

#include "ui/BioXAS/BioXASToolSuite/BioXASBeamlineTabWidget.h"

MotorToolMainScreen::MotorToolMainScreen(QWidget *parent) :
	QTabWidget(parent)
{
	setAttribute(Qt::WA_ShowWithoutActivating);
	resize(1190, 1600);

	setWindowTitle("BioXAS Motor Tool");

	setupUi();
}

MotorToolMainScreen::~MotorToolMainScreen()
{
}

void MotorToolMainScreen::setupUi()
{
	BioXASBeamlineTabWidget *tabMainBL = new BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASMainBeamLine);
	BioXASBeamlineTabWidget *tabSideBL = new BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASSideBeamLine);
	BioXASBeamlineTabWidget *tabImagingBL = new BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASImagingBeamLine);

	addTab(tabMainBL, "Main XAS BL");
	addTab(tabSideBL, "Side XAS BL");
	addTab(tabImagingBL, "Imaging BL");
	setCurrentIndex(0);
} // setupUi

