/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "IDEASPersistentView.h"

#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/IDEAS/IDEASScalerView.h"
#include "actions3/actions/AMWaitAction.h"
#include "source/StripTool2/IDEAS/IDEASScalerStripTool.h"


#include "ui/beamline/AMExtendedControlEditor.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QProgressBar>
#include <QGridLayout>
#include <QInputDialog>


#include <QDebug>


IDEASPersistentView::IDEASPersistentView(QWidget *parent) :
	QWidget(parent)
{
	stripTool_ = new IDEASScalerStripTool(this);
	//stripTool_->variables_->addVariable("BL08B2-1:mcs00:fbk");

	beamStatusLabel_ = new QLabel("Beam is off!");

	beamOnButton_ = new QPushButton("Beam On");
	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnClicked()));

	beamOffButton_ = new QPushButton("Beam Off");
	connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffClicked()));

	connect(IDEASBeamline::ideas(), SIGNAL(overallShutterStatus(bool)), this, SLOT(onShutterStatusChanged(bool)));

	calibrateButton_ = new QPushButton("Calibrate");
	connect(calibrateButton_, SIGNAL(clicked()), this, SLOT(onCalibrateClicked()));

	ringCurrent_ = new QLabel("     mA");
	ringCurrent_->setAlignment(Qt::AlignCenter);
	ringCurrent_->setFont(QFont("Lucida Grande", 16, QFont::Bold));

	energyControlEditor_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->monoEnergyControl());
	energyControlEditor_->setMaximumWidth(200);
	energyControlEditor_->setControlFormat('f', 1);

	monoCrystal_ = new QLabel("Crystal");
	monoCrystal_->setAlignment(Qt::AlignCenter);
	monoEnergyRange_ = new QLabel(" eV - eV");
	monoEnergyRange_->setAlignment(Qt::AlignLeft);

	//*****************TEST: MONO TROUBLESHOOTING TOOL, WILL NEED IN FUTURE
	//QDoubleSpinBox *monoSettlingTime_ = new QDoubleSpinBox();
	//connect(monoSettlingTime_, SIGNAL(valueChanged(double)), IDEASBeamline::ideas(), SLOT(setMonoSettlingTime(double)));
	//*****************ENDTEST

	IOldLabel_ = new QLabel("Old");
	I0Label_ = new QLabel("I_0");
	ISampleLabel_ = new QLabel("Sample");
	IReferenceLabel_ = new QLabel("Reference");

	IOldValueLabel_ = new QLabel("0");
	I0ValueLabel_ = new QLabel("0");
	ISampleValueLabel_ = new QLabel("0");
	IReferenceValueLabel_ = new QLabel("0");

	scalerContinuousButton_ = new QPushButton("Enable Continuous Updates");

	QGroupBox *jjSlitGroupBox = new QGroupBox;
	jjSlitGroupBox->setTitle("JJ Sample Slits");
	QGridLayout *jjSlitLayout = new QGridLayout;
	jjSlitHorizontalGap_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->jjSlitHGap());
	jjSlitHorizontalGap_->setControlFormat('f', 1);
	jjSlitLayout->addWidget(jjSlitHorizontalGap_,0,1,2,1);
	jjSlitHorizontalCenter_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->jjSlitHCenter());
	jjSlitHorizontalCenter_->setControlFormat('f', 1);
	jjSlitLayout->addWidget(jjSlitHorizontalCenter_,0,3,2,1);
	jjSlitVerticalGap_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->jjSlitVGap());
	jjSlitVerticalGap_->setControlFormat('f', 1);
	jjSlitLayout->addWidget(jjSlitVerticalGap_,2,1,2,1);
	jjSlitVerticalCenter_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->jjSlitVCenter());
	jjSlitVerticalCenter_->setControlFormat('f', 1);
	jjSlitLayout->addWidget(jjSlitVerticalCenter_,2,3,2,1);

	QIcon iconUp, iconDown, iconLeft, iconRight, iconVerticalClose, iconVerticalOpen, iconHorizontalClose, iconHorizontalOpen;
	iconUp.addFile(QString::fromUtf8(":/22x22/go-up-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconDown.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconLeft.addFile(QString::fromUtf8(":/22x22/go-previous-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconRight.addFile(QString::fromUtf8(":/22x22/go-next-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconVerticalClose.addFile(QString::fromUtf8(":/22x22/go-close-vert-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconVerticalOpen.addFile(QString::fromUtf8(":/22x22/go-open-vert-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconHorizontalClose.addFile(QString::fromUtf8(":/22x22/go-close-horiz-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconHorizontalOpen.addFile(QString::fromUtf8(":/22x22/go-open-horiz-dark.png"), QSize(), QIcon::Normal, QIcon::Off);


	jjSlitsVertOpen_ = new AMControlMoveButton();
	jjSlitsVertOpen_->setControl(IDEASBeamline::ideas()->jjSlitVGap());
	jjSlitsVertOpen_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsVertOpen_->setStepSizeIndex(2);
	jjSlitsVertOpen_->setIcon(iconVerticalOpen);
	jjSlitsVertOpen_->setIconSize(QSize(15, 15));
	jjSlitsVertOpen_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsVertOpen_->setMinimumWidth(15);
	jjSlitsVertOpen_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsVertOpen_,2,0,1,1);
	jjSlitsVertClose_ = new AMControlMoveButton();
	jjSlitsVertClose_->setControl(IDEASBeamline::ideas()->jjSlitVGap());
	jjSlitsVertClose_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsVertClose_->setStepSizeIndex(2);
	jjSlitsVertClose_->setDirectionReversed(true);
	jjSlitsVertClose_->setIcon(iconVerticalClose);
	jjSlitsVertClose_->setIconSize(QSize(15, 15));
	jjSlitsVertClose_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsVertClose_->setMinimumWidth(15);
	jjSlitsVertClose_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsVertClose_,3,0,1,1);


	jjSlitsHorizOpen_ = new AMControlMoveButton();
	jjSlitsHorizOpen_->setControl(IDEASBeamline::ideas()->jjSlitHGap());
	jjSlitsHorizOpen_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsHorizOpen_->setStepSizeIndex(2);
	jjSlitsHorizOpen_->setIcon(iconHorizontalOpen);
	jjSlitsHorizOpen_->setIconSize(QSize(15, 15));
	jjSlitsHorizOpen_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsHorizOpen_->setMinimumWidth(15);
	jjSlitsHorizOpen_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsHorizOpen_,0,0,1,1);
	jjSlitsHorizClose_ = new AMControlMoveButton();
	jjSlitsHorizClose_->setControl(IDEASBeamline::ideas()->jjSlitHGap());
	jjSlitsHorizClose_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsHorizClose_->setStepSizeIndex(2);
	jjSlitsHorizClose_->setDirectionReversed(true);
	jjSlitsHorizClose_->setIcon(iconHorizontalClose);
	jjSlitsHorizClose_->setIconSize(QSize(15, 15));
	jjSlitsHorizClose_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsHorizClose_->setMinimumWidth(15);
	jjSlitsHorizClose_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsHorizClose_,1,0,1,1);


	jjSlitsUp_ = new AMControlMoveButton();
	jjSlitsUp_->setControl(IDEASBeamline::ideas()->jjSlitVCenter());
	jjSlitsUp_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsUp_->setStepSizeIndex(2);
	jjSlitsUp_->setIcon(iconUp);
	jjSlitsUp_->setIconSize(QSize(15, 15));
	jjSlitsUp_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsUp_->setMinimumWidth(15);
	jjSlitsUp_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsUp_,2,2,1,1);
	jjSlitsDown_ = new AMControlMoveButton();
	jjSlitsDown_->setControl(IDEASBeamline::ideas()->jjSlitVCenter());
	jjSlitsDown_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsDown_->setStepSizeIndex(2);
	jjSlitsDown_->setDirectionReversed(true);
	jjSlitsDown_->setIcon(iconDown);
	jjSlitsDown_->setIconSize(QSize(15, 15));
	jjSlitsDown_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsDown_->setMinimumWidth(15);
	jjSlitsDown_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsDown_,3,2,1,1);


	jjSlitsLeft_ = new AMControlMoveButton();
	jjSlitsLeft_->setControl(IDEASBeamline::ideas()->jjSlitHCenter());
	jjSlitsLeft_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsLeft_->setStepSizeIndex(2);
	jjSlitsLeft_->setIcon(iconLeft);
	jjSlitsLeft_->setIconSize(QSize(15, 15));
	jjSlitsLeft_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsLeft_->setMinimumWidth(15);
	jjSlitsLeft_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsLeft_,0,2,1,1);
	jjSlitsRight_ = new AMControlMoveButton();
	jjSlitsRight_->setControl(IDEASBeamline::ideas()->jjSlitHCenter());
	jjSlitsRight_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
	jjSlitsRight_->setStepSizeIndex(2);
	jjSlitsRight_->setDirectionReversed(true);
	jjSlitsRight_->setIcon(iconRight);
	jjSlitsRight_->setIconSize(QSize(15, 15));
	jjSlitsRight_->setToolButtonStyle(Qt::ToolButtonIconOnly);
	jjSlitsRight_->setMinimumWidth(15);
	jjSlitsRight_->setMinimumHeight(15);
	jjSlitLayout->addWidget(jjSlitsRight_,1,2,1,1);

	jjSlitGroupBox->setLayout(jjSlitLayout);

	connect(IDEASBeamline::bl()->exposedControlByName("ringCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));

	connect(IDEASBeamline::ideas()->monoLowEV(),   SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
	connect(IDEASBeamline::ideas()->monoHighEV(),  SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
	connect(IDEASBeamline::ideas()->monoCrystal(), SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));

	KETEKstatusLabel_ = new QLabel;
	KETEKstatusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));
	KETEKstatusLabel_->setAlignment(Qt::AlignCenter);
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(initializationStateChanged(AMDetector::InitializationState)), this, SLOT(onInitializationStateChanged(AMDetector::InitializationState)));
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onAcquisitionStateChanged(AMDetector::AcqusitionState)));
	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(cleanupStateChanged(AMDetector::CleanupState)), this, SLOT(onCleanupStateChanged(AMDetector::CleanupState)));


	connect(IDEASBeamline::ideas()->monoLowEV(),   SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
	connect(IDEASBeamline::ideas()->monoHighEV(),  SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
	connect(IDEASBeamline::ideas()->monoCrystal(), SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));

	connect(scalerContinuousButton_, SIGNAL(clicked()), this, SLOT(onScalerContinuousButtonClicked()));

	QHBoxLayout *beamChangeLayout = new QHBoxLayout;
	beamChangeLayout->addWidget(beamOnButton_);
	beamChangeLayout->addWidget(beamOffButton_);

	QHBoxLayout *monoEnergyLayout = new QHBoxLayout;
	monoEnergyLayout->addWidget(energyControlEditor_);
	monoEnergyLayout->addWidget(monoCrystal_);
	monoEnergyLayout->addWidget(calibrateButton_);

	//QHBoxLayout *monoSetupLayout = new QHBoxLayout;
	//monoSetupLayout->addWidget(monoCrystal_);
	//monoSetupLayout->addWidget(monoEnergyRange_);

	QHBoxLayout *KETEKStatusLayout = new QHBoxLayout;
	KETEKStatusLayout->addWidget(new QLabel("KETEK Status: "),0, Qt::AlignRight);
	KETEKStatusLayout->addWidget(KETEKstatusLabel_);


	QVBoxLayout *mainPanelLayout = new QVBoxLayout;
	mainPanelLayout->addWidget(ringCurrent_);
	mainPanelLayout->addLayout(beamChangeLayout);
	mainPanelLayout->addWidget(beamStatusLabel_, 0, Qt::AlignCenter);
	mainPanelLayout->addLayout(monoEnergyLayout);
	//mainPanelLayout->addLayout(monoSetupLayout);
	mainPanelLayout->addLayout(KETEKStatusLayout);

	//mainPanelLayout->addWidget(monoSettlingTime_); //MONO TROUBLESHOOTING TOOL, WILL NEED IN FUTURE
	mainPanelLayout->addWidget(stripTool_);

	QVBoxLayout *scalerPanelLayout = new QVBoxLayout;
	scalerPanelLayout->addWidget(new IDEASScalerView());
	scalerPanelLayout->addWidget(scalerContinuousButton_);

	QGroupBox *persistentPanel = new QGroupBox("IDEAS Beamline");
	persistentPanel->setLayout(mainPanelLayout);

	QGroupBox *scalerPanel = new QGroupBox("Preamp Settings");
	scalerPanel->setLayout(scalerPanelLayout);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(persistentPanel);

	layout->addWidget(jjSlitGroupBox);
	layout->addWidget(scalerPanel);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	setMaximumHeight(1000);
	setMinimumHeight(800);
	setMaximumWidth(400);
	setMinimumWidth(400);

	setLayout(layout);
}

void IDEASPersistentView::onBeamOnClicked()
{
	AMAction3 *beamOn = IDEASBeamline::ideas()->createBeamOnAction();
	beamOn->start();
}

void IDEASPersistentView::onBeamOffClicked()
{
	AMAction3 *beamOff = IDEASBeamline::ideas()->createBeamOffAction();
	beamOff->start();
}

void IDEASPersistentView::onShutterStatusChanged(bool state)
{
	beamOnButton_->setDisabled(state);
	beamOffButton_->setEnabled(state);
	beamStatusLabel_->setText(state ? "Beam is on!" : "Beam is off!");
}


void IDEASPersistentView::onCrystalChanged()
{
	monoCrystal_->setText(QString("%1\n%2 eV -\n%3 eV").arg(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->enumNameAt(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->value())).arg(IDEASBeamline::ideas()->monoLowEV()->value()).arg(IDEASBeamline::ideas()->monoHighEV()->value()));
	//monoCrystal_->setText(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->enumNameAt(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->value()));
	//monoEnergyRange_->setText(QString("     %1 eV - %2 eV").arg(IDEASBeamline::ideas()->monoLowEV()->value()).arg(IDEASBeamline::ideas()->monoHighEV()->value()));
}

void IDEASPersistentView::onRingCurrentChanged(double current)
{
	ringCurrent_->setText(QString("%1 mA").arg(current));
	if(current < 10)
		ringCurrent_->setStyleSheet("QLabel { background-color : red; color : white}");
	else
		ringCurrent_->setStyleSheet("QLabel { color : black; }");
}



void IDEASPersistentView::onCalibrateClicked()
{
	bool ok;
	double newE = QInputDialog::getDouble(this,"Monochromator Energy Calibration","Enter Current Calibrated Energy:",IDEASBeamline::ideas()->monoEnergyControl()->value(),IDEASBeamline::ideas()->monoLowEV()->value(),IDEASBeamline::ideas()->monoHighEV()->value(),1,&ok);

	if(ok)
	{
		double oldAngleOffset = IDEASBeamline::ideas()->monoAngleOffset()->value();
		double currentE = IDEASBeamline::ideas()->monoEnergyControl()->value();
		double mono2d = IDEASBeamline::ideas()->mono2d()->value();
		double braggAngle = IDEASBeamline::ideas()->monoBraggAngle()->value();

		double dE = newE - currentE;

		double angleDetla = -12398.4193 / (mono2d * currentE * currentE * cos(braggAngle * M_PI / 180)) * dE * 180 / M_PI;

		IDEASBeamline::ideas()->monoAngleOffset()->move(oldAngleOffset + angleDetla);

	}
}

void IDEASPersistentView::onScalerContinuousButtonClicked()
{
	AMListAction3 *scalerContinuousEnableActions = new AMListAction3(new AMListActionInfo3("Enable Scaler Continuous Mode", "Enable Scaler Continuous Mode"));
	scalerContinuousEnableActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.5)));
	scalerContinuousEnableActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(false));
	scalerContinuousEnableActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.5)));
	scalerContinuousEnableActions->addSubAction(IDEASBeamline::ideas()->scaler()->createDwellTimeAction3(0.25));
	scalerContinuousEnableActions->addSubAction(IDEASBeamline::ideas()->scaler()->createTotalScansAction3(0));
	scalerContinuousEnableActions->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.5)));
	scalerContinuousEnableActions->addSubAction(IDEASBeamline::ideas()->scaler()->createStartAction3(true));

	scalerContinuousEnableActions->start();
}

void IDEASPersistentView::onAcquisitionStateChanged(AMDetector::AcqusitionState state)
{
	bool isAcquiring = state == AMDetector::Acquiring;
	KETEKstatusLabel_->setPixmap(QIcon(isAcquiring ? ":/32x32/greenLEDOn.png" : ":/32x32/greenLEDOff.png").pixmap(22));
}
