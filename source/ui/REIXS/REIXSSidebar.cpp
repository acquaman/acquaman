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


#include "REIXSSidebar.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QSpacerItem>

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRunner3.h"

#include "beamline/REIXS/REIXSBeamline.h"

#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSActionBasedControlEditor.h"
#include "ui/REIXS/REIXSScalerView.h"


REIXSSidebar::REIXSSidebar(QWidget *parent) :
	QWidget(parent)
{
	setObjectName("REIXSSidebar");
	resize(266, 529);
	setupUi();
	setupConnections();

	// Get initial status:
	//////////////////////////
	onBeamOnChanged(REIXSBeamline::bl()->valvesAndShutters()->isBeamOn());
}

REIXSSidebar::~REIXSSidebar()
{
}

void REIXSSidebar::onMCPCountsPerSecondChanged(double countsPerSecond)
{
	XESValue_->setText("XES\t\t" + QLocale(QLocale::English).toString(countsPerSecond, 'f', 0) + " counts");
}

void REIXSSidebar::onBeamOnButtonClicked()
{
	AMAction3 *beamOn = REIXSBeamline::bl()->buildBeamStateChangeAction(true);
	connect(beamOn, SIGNAL(succeeded()), beamOn, SLOT(deleteLater()));
	connect(beamOn, SIGNAL(cancelled()), beamOn, SLOT(deleteLater()));
	connect(beamOn, SIGNAL(failed()), beamOn, SLOT(deleteLater()));
	beamOn->start();
}

void REIXSSidebar::onBeamOffButtonClicked()
{
	AMAction3 *beamOff = REIXSBeamline::bl()->buildBeamStateChangeAction(false);
	connect(beamOff, SIGNAL(succeeded()), beamOff, SLOT(deleteLater()));
	connect(beamOff, SIGNAL(cancelled()), beamOff, SLOT(deleteLater()));
	connect(beamOff, SIGNAL(failed()), beamOff, SLOT(deleteLater()));
	beamOff->start();
}

void REIXSSidebar::onBeamOnChanged(bool isOn)
{
	if(isOn) {
		beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		beamOnButton_->setChecked(true);
	}
	else {
		beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		beamOffButton_->setChecked(true);
	}
}

void REIXSSidebar::onTFYCountsChanged(int counts)
{
	double counts_ = counts;
	TFYValue_->setText("TFY\t\t" + QLocale(QLocale::English).toString(counts_, 'f', 0) + " counts");

}

void REIXSSidebar::onScalerContinuousButtonToggled(bool on)
{
	REIXSBeamline::bl()->scaler()->setContinuous(on);
}

void REIXSSidebar::onScalerConnected()
{
	enableScalerContinuousButton_->setChecked(REIXSBeamline::bl()->scaler()->isContinuous());
}

void REIXSSidebar::onScalerContinuousModeChanged(bool on)
{
	enableScalerContinuousButton_->setChecked(on);
}

void REIXSSidebar::on_MonoStopButton_clicked()
{
	AMActionRunner3::workflow()->cancelCurrentAction();
	REIXSBeamline::bl()->photonSource()->energy()->stop();
}

void REIXSSidebar::onRingCurrentChanged(double current)
{
	ringCurrentValue_->setText((QString("%1 mA").arg(int(current))));
}

void REIXSSidebar::setupUi()
{
	beamlineGroupBox_ = new QGroupBox("Beamline");
	layoutBeamlineContent();

	detectorsGroupBox_ = new QGroupBox("Detector Signals:");
	layoutDetectorContent();

	QVBoxLayout *verticalLayout = new QVBoxLayout();
	verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
	verticalLayout->addWidget(beamlineGroupBox_);
	verticalLayout->addStretch();
	verticalLayout->addWidget(detectorsGroupBox_);
	setLayout(verticalLayout);
}

void REIXSSidebar::setupConnections()
{
	connect(REIXSBeamline::bl()->valvesAndShutters(), SIGNAL(beamOnChanged(bool)), this, SLOT(onBeamOnChanged(bool)));
	connect(REIXSBeamline::bl()->mcpDetector(), SIGNAL(countsPerSecondChanged(double)), this, SLOT(onMCPCountsPerSecondChanged(double)));

	connect(REIXSBeamline::bl()->scaler()->channelAt(4), SIGNAL(readingChanged(int)), this, SLOT(onTFYCountsChanged(int)));

	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));

	connect(enableScalerContinuousButton_, SIGNAL(clicked(bool)), this, SLOT(onScalerContinuousButtonToggled(bool)));
	connect(REIXSBeamline::bl()->scaler(), SIGNAL(continuousChanged(bool)), this, SLOT(onScalerContinuousModeChanged(bool)));
    connect(REIXSBeamline::bl()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()));


	connect(MonoStopButton_, SIGNAL(clicked()), this, SLOT(on_MonoStopButton_clicked()));

	connect(REIXSBeamline::bl()->photonSource()->ringCurrent(), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));
}

void REIXSSidebar::layoutBeamlineContent()
{

	QFont font;
	font.setPointSize(14);
	font.setBold(true);
	font.setWeight(75);

	ringCurrentValue_ = new QLabel("250 mA");
	ringCurrentValue_->setMinimumSize(QSize(78, 0));
	ringCurrentValue_->setMaximumSize(QSize(78, 16777215));
	ringCurrentValue_->setFont(font);

	beamOnButton_ = createPushButton("On");
	beamOffButton_ = createPushButton("Off");
	beamlineStatusLED_ = new QLabel();
	beamlineStatusLED_->setObjectName("beamlineStatusLED");
	beamlineStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(0);
	horizontalLayout->setContentsMargins(-1, 0, -1, 0);
	horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	horizontalLayout->addWidget(beamOnButton_);
	horizontalLayout->addWidget(beamOffButton_);
	horizontalLayout->addItem(new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Minimum));
	horizontalLayout->addWidget(beamlineStatusLED_);

	beamlineEnergyEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->energy());
	userEnergyOffestEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->userEnergyOffset());
	gratingSelector_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoGratingSelector());
	gratingSelector_->setEnabled(false);
	mirrorSelector_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoMirrorSelector());
	mirrorSelector_->setEnabled(false);
	monoSlitEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoSlit());
	epuPolarizationEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->epuPolarization());
	epuPolarizationAngleEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->epuPolarizationAngle());

	QIcon icon;
	icon.addFile(":/Close.png", QSize(), QIcon::Normal, QIcon::Off);
	MonoStopButton_ = new QPushButton(icon, "Stop!");

	QFormLayout *beamlineFormLayout = new QFormLayout();
	beamlineFormLayout->setContentsMargins(6, 6, 6, 6);
	beamlineFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	beamlineFormLayout->setWidget(0, QFormLayout::LabelRole, ringCurrentValue_);
	beamlineFormLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout);
	beamlineFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Energy"));
	beamlineFormLayout->setWidget(1, QFormLayout::FieldRole, beamlineEnergyEditor_);
	beamlineFormLayout->setWidget(2, QFormLayout::LabelRole, new QLabel("user offset"));
	beamlineFormLayout->setWidget(2, QFormLayout::FieldRole, userEnergyOffestEditor_);
	beamlineFormLayout->setWidget(3, QFormLayout::LabelRole, new QLabel("Grating"));
	beamlineFormLayout->setWidget(3, QFormLayout::FieldRole, gratingSelector_);
	beamlineFormLayout->setWidget(4, QFormLayout::LabelRole, new QLabel("Mirror"));
	beamlineFormLayout->setWidget(4, QFormLayout::FieldRole, mirrorSelector_);
	beamlineFormLayout->setWidget(5, QFormLayout::LabelRole, new QLabel("Exit slit"));
	beamlineFormLayout->setWidget(5, QFormLayout::FieldRole, monoSlitEditor_);
	beamlineFormLayout->setWidget(6, QFormLayout::LabelRole, new QLabel("Polarization"));
	beamlineFormLayout->setWidget(6, QFormLayout::FieldRole, epuPolarizationEditor_);
	beamlineFormLayout->setWidget(7, QFormLayout::LabelRole, new QLabel("Angle"));
	beamlineFormLayout->setWidget(7, QFormLayout::FieldRole, epuPolarizationAngleEditor_);
	beamlineFormLayout->setWidget(8, QFormLayout::SpanningRole, MonoStopButton_);

	beamlineGroupBox_->setLayout(beamlineFormLayout);
}

void REIXSSidebar::layoutDetectorContent()
{
	XESValue_ = new QLabel("XES:\t\t0 counts");
	XESValue_->setFixedHeight(55);
	TFYValue_ = new QLabel("TFY:\t\t0 counts");
	TFYValue_->setFixedHeight(55);
	enableScalerContinuousButton_ = new QPushButton("Monitor Counts");
	enableScalerContinuousButton_->setCheckable(true);
	enableScalerContinuousButton_->setChecked(REIXSBeamline::bl()->scaler()->isContinuous());


	QVBoxLayout *detectorPanelLayout = new QVBoxLayout();
	detectorPanelLayout->addWidget(XESValue_);
	detectorPanelLayout->addWidget(TFYValue_);
	detectorPanelLayout->addWidget(new REIXSScalerView());
	detectorPanelLayout->addWidget(enableScalerContinuousButton_);

	detectorsGroupBox_->setLayout(detectorPanelLayout);
}

QPushButton *REIXSSidebar::createPushButton(QString text)
{
	QPushButton *pushButton = new QPushButton();
	pushButton->setMaximumSize(QSize(55, 16777215));
	pushButton->setCheckable(true);
	pushButton->setAutoExclusive(true);
	pushButton->setText(text);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
	pushButton->setSizePolicy(sizePolicy);

	return pushButton;
}
