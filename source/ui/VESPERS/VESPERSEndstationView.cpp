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


#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/AMTopFrame.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QGridLayout>
#include <QList>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFile>
#include <QTextStream>
#include <QToolButton>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>

VESPERSEndstationView::VESPERSEndstationView(VESPERSEndstation *endstation, QWidget *parent)
	: QWidget(parent)
{
	// Setup the top frame.
	AMTopFrame *topFrame = new AMTopFrame("Endstation Control Screen");

	// The endstation model.
	endstation_ = endstation;

	// Setup the buttons used in the picture.
	ccdButton_ = new QToolButton;
	connect(ccdButton_, SIGNAL(clicked()), this, SLOT(ccdClicked()));
	connect(endstation_, SIGNAL(ccdFbkChanged(double)), this, SLOT(ccdUpdate(double)));
	microscopeButton_ = new QToolButton;
	connect(microscopeButton_, SIGNAL(clicked()), this, SLOT(microscopeClicked()));
	connect(endstation_, SIGNAL(microscopeFbkChanged(double)), this, SLOT(microscopeUpdate(double)));
	fourElButton_ = new QToolButton;
	connect(fourElButton_, SIGNAL(clicked()), this, SLOT(fourElClicked()));
	connect(endstation_, SIGNAL(fourElFbkChanged(double)), this, SLOT(fourElUpdate(double)));
	singleElButton_ = new QToolButton;
	connect(singleElButton_, SIGNAL(clicked()), this, SLOT(singleElClicked()));
	connect(endstation_, SIGNAL(singleElFbkChanged(double)), this, SLOT(singleElUpdate(double)));

	// Setup the microscope light.  Tracking needs to be off!  Otherwise, the program might get into an infinite signal slot loop.
	micLight_ = new QSlider(Qt::Vertical, this);
	micLight_->setMinimum(0);
	micLight_->setMaximum(100);
	micLight_->setTickInterval(10);
	micLight_->setTickPosition(QSlider::TicksRight);
	micLight_->setTracking(false);
	connect(endstation_, SIGNAL(lightIntensityChanged(int)), this, SLOT(setMicroscopeLight(int)));
	connect(micLight_, SIGNAL(valueChanged(int)), endstation_, SLOT(setLightIntensity(int)));

	lightBulb_ = new QToolButton;
	lightBulb_->setIcon(QIcon(":/lightbulb.png"));
	lightBulb_->setCheckable(true);
	connect(lightBulb_, SIGNAL(toggled(bool)), this, SLOT(lightBulbToggled(bool)));

	// Setup the GUI with the soft limits.
	config_ = new VESPERSEndstationLimitsView;
	config_->hide();
	connect(config_, SIGNAL(configurationChanged()), endstation_, SLOT(loadConfiguration()));

	QToolButton *configButton = new QToolButton;
	configButton->setIcon(QIcon(":/configure.png"));
	connect(configButton, SIGNAL(clicked()), config_, SLOT(show()));
	connect(configButton, SIGNAL(clicked()), config_, SLOT(raise()));

	// Laser power signal.
	laserPowerButton_ = new QToolButton;
	laserPowerButton_->setIcon(QIcon(":/red-laser.png"));
	laserPowerButton_->setCheckable(true);
	laserPowerButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	QFont font(this->font());
	font.setBold(true);
	laserPowerButton_->setFont(font);
	connect(laserPowerButton_, SIGNAL(clicked()), endstation_, SLOT(toggleLaserPower()));
	connect(endstation_, SIGNAL(laserPoweredChanged()), this, SLOT(laserPowerUpdate()));
	connect(endstation_, SIGNAL(laserPositionChanged(double)), this, SLOT(onLaserDistanceChanged()));
	connect(endstation_, SIGNAL(laserPositionValidityChanged(bool)), this, SLOT(onLaserDistanceChanged()));

	// Main control group box setup.
	QGroupBox *controlGB = new QGroupBox;
	controlGB->setObjectName("Control");
	controlGB->setFixedSize(640, 400);
	controlGB->setStyleSheet("QGroupBox#Control { background-image: url(:/VESPERS/Endstation-background.png); }");
	QGridLayout *controlGBLayout = new QGridLayout;
	for (int i = 0; i < 20; i++){

		controlGBLayout->addItem(new QSpacerItem(30, 30), i, 0);
		controlGBLayout->addItem(new QSpacerItem(30, 30), 0, i);
	}
	controlGBLayout->addWidget(ccdButton_, 7, 8, 2, 3);
	controlGBLayout->addWidget(microscopeButton_, 9, 10, 2, 3);
	controlGBLayout->addWidget(singleElButton_, 15, 5, 2, 3);
	controlGBLayout->addWidget(fourElButton_, 16, 11, 2, 3);
	controlGBLayout->addWidget(lightBulb_, 7, 5, 2, 2);
	controlGBLayout->addWidget(micLight_, 2, 0, 8, 1);
	controlGBLayout->addWidget(configButton, 0, 0, 2, 1);
	controlGBLayout->addWidget(laserPowerButton_, 10, 13, 2, 5);
	controlGB->setLayout(controlGBLayout);

	window_ = new VESPERSMotorView(this);
	QVBoxLayout *windowGBLayout = new QVBoxLayout;
	windowGBLayout->addWidget(window_);
	QGroupBox *windowGB = new QGroupBox(tr("Motor Control"));
	windowGB->setMinimumSize(280, 145);
	windowGB->setLayout(windowGBLayout);
	connect(endstation_, SIGNAL(currentControlChanged(AMControl*)), this, SLOT(setWindow(AMControl*)));
	endstation_->setCurrent("1-Element Vortex motor");

	QGridLayout *endstationLayout = new QGridLayout;
	endstationLayout->addWidget(controlGB, 0, 0, 3, 3);
	endstationLayout->addWidget(windowGB, 0, 3);

	QHBoxLayout *squishLayout = new QHBoxLayout;
	squishLayout->addStretch();
	squishLayout->addLayout(endstationLayout);
	squishLayout->addStretch();

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addWidget(topFrame);
	masterLayout->addStretch();
	masterLayout->addLayout(squishLayout);
	masterLayout->addStretch();

	setLayout(masterLayout);
	setMinimumSize(1100, 465);
}

VESPERSEndstationView::~VESPERSEndstationView()
{
	config_->deleteLater();
}

void VESPERSEndstationView::onLaserDistanceChanged()
{
	QPalette palette(this->palette());

	if (endstation_->laserPowered() && endstation_->laserPositionValid()){

		palette.setBrush(QPalette::ButtonText, Qt::darkGreen);
		laserPowerButton_->setPalette(palette);
		laserPowerButton_->setText(QString("%1 mm").arg(endstation_->laserPosition(), 0, 'f', 3));
	}

	else if (endstation_->laserPowered()){

		palette.setBrush(QPalette::ButtonText, Qt::red);
		laserPowerButton_->setPalette(palette);
		laserPowerButton_->setText("-FFFFFF");
	}

	else {

		palette.setBrush(QPalette::ButtonText, Qt::black);
		laserPowerButton_->setPalette(palette);
		laserPowerButton_->setText("OFF");
	}
}

void VESPERSEndstationView::setWindow(AMControl *control)
{
	if (control == 0)
		return;

	QString name(control->name());
	AMRange range(endstation_->getLimits(control));

	if (name == "CCD motor" || name == "1-Element Vortex motor" || name == "4-Element Vortex motor")
		window_->setControl(control, range.minimum(), range.maximum());
	else if (name == "Microscope motor")
		window_->setControl(control, range.minimum(), range.maximum(), endstation_->microscopeNames().first, endstation_->microscopeNames().second);
}

void VESPERSEndstationView::laserPowerUpdate()
{
	laserPowerButton_->blockSignals(true);

	if (endstation_->laserPowered()){

		laserPowerButton_->setIcon(QIcon(":/red-laser.png"));
		laserPowerButton_->setChecked(false);
	}
	else{

		laserPowerButton_->setIcon(QIcon(":/black-laser.png"));
		laserPowerButton_->setChecked(true);
	}

	laserPowerButton_->blockSignals(false);
	onLaserDistanceChanged();
}

void VESPERSEndstationView::lightBulbToggled(bool pressed)
{
	if (pressed){

		disconnect(endstation_, SIGNAL(lightIntensityChanged(int)), this, SLOT(setMicroscopeLight(int)));
		disconnect(micLight_, SIGNAL(valueChanged(int)), endstation_, SLOT(setLightIntensity(int)));
		endstation_->setLightIntensity(0);
		lightBulb_->setIcon(QIcon(":/dialog-information.png"));
	}
	else{

		connect(endstation_, SIGNAL(lightIntensityChanged(int)), this, SLOT(setMicroscopeLight(int)));
		connect(micLight_, SIGNAL(valueChanged(int)), endstation_, SLOT(setLightIntensity(int)));
		endstation_->setLightIntensity(micLight_->value());
		lightBulb_->setIcon(QIcon(":/lightbulb.png"));
	}

}

///////////////////////////////////////////////////////////////////////
// Endstation Configuration

 VESPERSEndstationLimitsView::~VESPERSEndstationLimitsView(){}
VESPERSEndstationLimitsView::VESPERSEndstationLimitsView(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("Endstation Configuration");

	// Initializing.
	ccdLowLimit_ = new QLineEdit;
	ccdHighLimit_ = new QLineEdit;
	ccdHomePosition_ = new QLineEdit;

	microscopeInPosition_ = new QLineEdit;
	microscopeOutPosition_ = new QLineEdit;
	microscopeInStatus_ = new QLineEdit;
	microscopeOutStatus_ = new QLineEdit;

	vortexLowLimit_ = new QLineEdit;
	vortexHighLimit_ = new QLineEdit;
	vortexHomePosition_ = new QLineEdit;

	vortex4LowLimit_ = new QLineEdit;
	vortex4HighLimit_ = new QLineEdit;
	vortex4HomePosition_ = new QLineEdit;

	ccdBufferSafePosition_ = new QLineEdit;
	usingBuffer_ = new QCheckBox;
	at90Deg_ = new QCheckBox;

	// Aligning.
	ccdLowLimit_->setAlignment(Qt::AlignCenter);
	ccdHighLimit_->setAlignment(Qt::AlignCenter);
	ccdHomePosition_->setAlignment(Qt::AlignCenter);

	microscopeInPosition_->setAlignment(Qt::AlignCenter);
	microscopeOutPosition_->setAlignment(Qt::AlignCenter);
	microscopeInStatus_->setAlignment(Qt::AlignCenter);
	microscopeOutStatus_->setAlignment(Qt::AlignCenter);

	vortexLowLimit_->setAlignment(Qt::AlignCenter);
	vortexHighLimit_->setAlignment(Qt::AlignCenter);
	vortexHomePosition_->setAlignment(Qt::AlignCenter);

	vortex4LowLimit_->setAlignment(Qt::AlignCenter);
	vortex4HighLimit_->setAlignment(Qt::AlignCenter);
	vortex4HomePosition_->setAlignment(Qt::AlignCenter);

	ccdBufferSafePosition_->setAlignment(Qt::AlignCenter);
	usingBuffer_->setText("Helium Buffer");
	at90Deg_->setText("CCD at 90 deg");

	// Setting up the group box layouts.
	QGroupBox *ccdGB = new QGroupBox("CCD Setup");
	QGridLayout *ccdLowLayout = new QGridLayout;
	ccdLowLayout->addWidget(ccdLowLimit_, 0, 0, 1, 3);
	ccdLowLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *ccdHighLayout = new QGridLayout;
	ccdHighLayout->addWidget(ccdHighLimit_, 0, 0, 1, 3);
	ccdHighLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *ccdHomeLayout = new QGridLayout;
	ccdHomeLayout->addWidget(ccdHomePosition_, 0, 0, 1, 3);
	ccdHomeLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *ccdBufferSafeLayout = new QGridLayout;
	ccdBufferSafeLayout->addWidget(ccdBufferSafePosition_, 0, 0, 1, 3);
	ccdBufferSafeLayout->addWidget(new QLabel(tr("mm")), 0, 2);

	QFormLayout *ccdLayout = new QFormLayout;
	ccdLayout->addRow("Low Limit", ccdLowLayout);
	ccdLayout->addRow("High Limit", ccdHighLayout);
	ccdLayout->addRow("Home Position", ccdHomeLayout);
	ccdLayout->addRow("Safe w/ Buffer Position", ccdBufferSafeLayout);
	ccdLayout->addRow(usingBuffer_, at90Deg_);
	ccdGB->setLayout(ccdLayout);

	QGroupBox *microscopeGB = new QGroupBox("Microscope Setup");
	QGridLayout *microscopeInLayout = new QGridLayout;
	microscopeInLayout->addWidget(microscopeInPosition_, 0, 0, 1, 3);
	microscopeInLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *microscopeOutLayout = new QGridLayout;
	microscopeOutLayout->addWidget(microscopeOutPosition_, 0, 0, 1, 3);
	microscopeOutLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QHBoxLayout *microscopeInStatusLayout = new QHBoxLayout;
	microscopeInStatusLayout->addWidget(microscopeInStatus_);
	QHBoxLayout *microscopeOutStatusLayout = new QHBoxLayout;
	microscopeOutStatusLayout->addWidget(microscopeOutStatus_);
	QFormLayout *microscopeLayout = new QFormLayout;
	microscopeLayout->addRow("In Position", microscopeInLayout);
	microscopeLayout->addRow("Out Position", microscopeOutLayout);
	microscopeLayout->addRow("In Status", microscopeInStatusLayout);
	microscopeLayout->addRow("Out Status", microscopeOutStatusLayout);
	microscopeGB->setLayout(microscopeLayout);

	QGroupBox *vortexGB = new QGroupBox("Vortex Setup");
	QGridLayout *vortexLowLayout = new QGridLayout;
	vortexLowLayout->addWidget(vortexLowLimit_, 0, 0, 1, 3);
	vortexLowLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *vortexHighLayout = new QGridLayout;
	vortexHighLayout->addWidget(vortexHighLimit_, 0, 0, 1, 3);
	vortexHighLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *vortexHomeLayout = new QGridLayout;
	vortexHomeLayout->addWidget(vortexHomePosition_, 0, 0, 1, 3);
	vortexHomeLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QFormLayout *vortexLayout = new QFormLayout;
	vortexLayout->addRow("Low Limit", vortexLowLayout);
	vortexLayout->addRow("High Limit", vortexHighLayout);
	vortexLayout->addRow("Home Position", vortexHomeLayout);
	vortexGB->setLayout(vortexLayout);

	QGroupBox *vortex4GB = new QGroupBox("4-Element Vortex Setup");
	QGridLayout *vortex4LowLayout = new QGridLayout;
	vortex4LowLayout->addWidget(vortex4LowLimit_, 0, 0, 1, 3);
	vortex4LowLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *vortex4HighLayout = new QGridLayout;
	vortex4HighLayout->addWidget(vortex4HighLimit_, 0, 0, 1, 3);
	vortex4HighLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QGridLayout *vortex4HomeLayout = new QGridLayout;
	vortex4HomeLayout->addWidget(vortex4HomePosition_, 0, 0, 1, 3);
	vortex4HomeLayout->addWidget(new QLabel(tr("mm")), 0, 2);
	QFormLayout *vortex4Layout = new QFormLayout;
	vortex4Layout->addRow("Low Limit", vortex4LowLayout);
	vortex4Layout->addRow("High Limit", vortex4HighLayout);
	vortex4Layout->addRow("Home Position", vortex4HomeLayout);
	vortex4GB->setLayout(vortex4Layout);

	QPushButton *saveButton = new QPushButton(QIcon(":/save.png"), tr("Save"));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveFile()));
	QHBoxLayout *saveButtonLayout = new QHBoxLayout;
	saveButtonLayout->addWidget(saveButton, 0, Qt::AlignRight);

	QVBoxLayout *configLayout = new QVBoxLayout;
	configLayout->addWidget(ccdGB);
	configLayout->addWidget(microscopeGB);
	configLayout->addWidget(vortexGB);
	configLayout->addWidget(vortex4GB);
	configLayout->addLayout(saveButtonLayout);

	loadFile();

	setLayout(configLayout);
}

void VESPERSEndstationLimitsView::saveFile()
{
	QString filePath = VESPERS::getHomeDirectory() % "/acquaman/build/endstation.config";
	QFile file(filePath);

	if (!file.open(QFile::WriteOnly | QFile::Text)){
		QMessageBox::warning(this, tr("Endstation Configuration"),
							 tr("Cannot save file %1: \n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
				return;
	}

	QTextStream out(&file);

	out << "# Order is Low Limit, High Limit, Home Position.  Microscope is In, Out.\n";
	out << "CCD\n";
	out << ccdLowLimit_->text() + "\n";
	out << ccdHighLimit_->text() + "\n";
	out << ccdHomePosition_->text() + "\n";
	out << "Vortex\n";
	out << vortexLowLimit_->text() + "\n";
	out << vortexHighLimit_->text() + "\n";
	out << vortexHomePosition_->text() + "\n";
	out << "4 Element Vortex\n";
	out << vortex4LowLimit_->text() + "\n";
	out << vortex4HighLimit_->text() + "\n";
	out << vortex4HomePosition_->text() + "\n";
	out << "Microscope\n";
	out << microscopeInPosition_->text() + "\n";
	out << microscopeOutPosition_->text() + "\n";
	out << microscopeInStatus_->text() + "\n";
	out << microscopeOutStatus_->text() + "\n";
	out << "CCD Extras\n";
	out << ccdBufferSafePosition_->text() + "\n";
	out << (usingBuffer_->isChecked() ? "YES\n" : "NO\n");
	out << (at90Deg_->isChecked() ? "YES\n" : "NO\n");

	file.close();

	hide();

	emit configurationChanged();
}

void VESPERSEndstationLimitsView::loadFile()
{
	QString filePath = VESPERS::getHomeDirectory() % "/acquaman/build/endstation.config";
	QFile file(filePath);

	// If there is no configuration file, then it creates a file with some default values.
	if (!file.open(QFile::ReadOnly | QFile::Text)){

		ccdLowLimit_->setText(QString::number(100, 'f', 1));
		ccdHighLimit_->setText(QString::number(230, 'f', 1));
		ccdHomePosition_->setText(QString::number(230, 'f', 1));

		vortexLowLimit_->setText(QString::number(60, 'f', 1));
		vortexHighLimit_->setText(QString::number(180, 'f', 1));
		vortexHomePosition_->setText(QString::number(180, 'f', 1));

		vortex4LowLimit_->setText(QString::number(60, 'f', 1));
		vortex4HighLimit_->setText(QString::number(180, 'f', 1));
		vortex4HomePosition_->setText(QString::number(180, 'f', 1));

		microscopeInPosition_->setText(QString::number(90, 'f', 1));
		microscopeOutPosition_->setText(QString::number(190, 'f', 1));
		microscopeInStatus_->setText("In");
		microscopeOutStatus_->setText("Out");

		ccdBufferSafePosition_->setText(QString::number(350, 'f', 1));
		usingBuffer_->setChecked(true);
		at90Deg_->setChecked(true);

		saveFile();

		return;
	}

	QTextStream in(&file);
	QStringList contents;

	while(!in.atEnd())
		contents << in.readLine();

	file.close();

	ccdLowLimit_->setText(contents.at(2));
	ccdHighLimit_->setText(contents.at(3));
	ccdHomePosition_->setText(contents.at(4));

	vortexLowLimit_->setText(contents.at(6));
	vortexHighLimit_->setText(contents.at(7));
	vortexHomePosition_->setText(contents.at(8));

	vortex4LowLimit_->setText(contents.at(10));
	vortex4HighLimit_->setText(contents.at(11));
	vortex4HomePosition_->setText(contents.at(12));

	microscopeInPosition_->setText(contents.at(14));
	microscopeOutPosition_->setText(contents.at(15));
	microscopeInStatus_->setText(contents.at(16));
	microscopeOutStatus_->setText(contents.at(17));

	ccdBufferSafePosition_->setText(contents.at(19));
	usingBuffer_->setChecked(contents.at(20) == "YES");
	at90Deg_->setChecked(contents.at(21) == "YES");
}
