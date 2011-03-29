#include "ui/VESPERS/VESPERSEndstationView.h"
#include "ui/AMStopButton.h"
#include "ui/AMShutterButton.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QPushButton>
#include <QGridLayout>
#include <QDebug>
#include <QList>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFile>
#include <QTextStream>
#include <QToolBar>
#include <QAction>
#include <QDir>
#include <QLabel>
#include <QMessageBox>

VESPERSEndstationView::VESPERSEndstationView(QWidget *parent)
	: QWidget(parent)
{
	// The controls.
	ccdControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->ccdMotor());
	microscopeControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->microscopeMotor());
	fourElControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->fourElMotor());
	singleElControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->singleElMotor());
	focusControl_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->focusMotor());

	// The feedback PVs associated with the controls.
	ccdfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->ccdMotorfbk());
	fourElfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->fourElMotorfbk());
	singleElfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->singleElMotorfbk());
	focusfbk_ = qobject_cast<AMReadOnlyPVControl *>(VESPERSBeamline::vespers()->focusMotorfbk());

	// The microscope light and CCD file path PVs.
	micLightPV_ = VESPERSBeamline::vespers()->micLight();
	ccdPath_ = VESPERSBeamline::vespers()->ccdPath();
	ccdFile_ = VESPERSBeamline::vespers()->ccdFile();
	ccdNumber_ = VESPERSBeamline::vespers()->ccdNumber();

	// The shutter buttons.
	AMShutterButton *psh1 = new AMShutterButton("PSH1", "PSH1408-B20-01:state", "PSH1408-B20-01:opr:open", "PSH1408-B20-01:opr:close");
	AMShutterButton *psh2 = new AMShutterButton("PSH2", "PSH1408-B20-02:state", "PSH1408-B20-02:opr:open", "PSH1408-B20-02:opr:close");
	AMShutterButton *ssh1 = new AMShutterButton("SSH1", "SSH1408-B20-01:state");
	AMShutterButton *ssh2 = new AMShutterButton("SSH2", "SSH1607-1-B21-01:state");

	// Get the current soft limits.
	loadConfiguration();

	// Setting the flags to false as a precaution.
	microscopeSafe_ = false;
	ccdSafe_ = false;

	// Setup the buttons used in the picture.
	ccdButton_ = new QToolButton;
	connect(ccdButton_, SIGNAL(clicked()), this, SLOT(ccdClicked()));
	connect(ccdfbk_, SIGNAL(valueChanged(double)), this, SLOT(ccdUpdate(double)));
	microscopeButton_ = new QToolButton;
	connect(microscopeButton_, SIGNAL(clicked()), this, SLOT(microscopeClicked()));
	connect(microscopeControl_, SIGNAL(valueChanged(double)), this, SLOT(microscopeUpdate(double)));
	fourElButton_ = new QToolButton;
	connect(fourElButton_, SIGNAL(clicked()), this, SLOT(fourElClicked()));
	connect(fourElfbk_, SIGNAL(valueChanged(double)), this, SLOT(fourElUpdate(double)));
	singleElButton_ = new QToolButton;
	connect(singleElButton_, SIGNAL(clicked()), this, SLOT(singleElClicked()));
	connect(singleElfbk_, SIGNAL(valueChanged(double)), this, SLOT(singleElUpdate(double)));
	focusButton_ = new QToolButton;
	connect(focusButton_, SIGNAL(clicked()), this, SLOT(focusClicked()));
	connect(focusfbk_, SIGNAL(valueChanged(double)), this, SLOT(focusUpdate(double)));

	// Setup the microscope light.
	micLight_ = new QSlider(Qt::Vertical, this);
	micLight_->setMinimum(0);
	micLight_->setMaximum(100);
	micLight_->setTickInterval(10);
	micLight_->setTickPosition(QSlider::TicksRight);
	connect(micLightPV_, SIGNAL(valueChanged()), this, SLOT(micLightUpdate()));
	connect(micLight_, SIGNAL(valueChanged(int)), micLightPV_, SLOT(setValue(int)));

	QToolButton *lightBulb = new QToolButton;
	lightBulb->setIcon(QIcon(":/lightbulb.png"));
	lightBulb->setCheckable(true);
	connect(lightBulb, SIGNAL(toggled(bool)), this, SLOT(lightBulbToggled(bool)));

	// Main control group box setup.
	QGroupBox *controlGB = new QGroupBox;
	controlGB->setObjectName("Control");
	controlGB->setFixedSize(640, 400);
	controlGB->setStyleSheet("QGroupBox#Control { background-image: url(:/Endstation-background.png); }");
	QGridLayout *controlGBLayout = new QGridLayout;
	for (int i = 0; i < 20; i++){

		controlGBLayout->addItem(new QSpacerItem(30, 30), i, 0);
		controlGBLayout->addItem(new QSpacerItem(30, 30), 0, i);
	}
	controlGBLayout->addWidget(ccdButton_, 7, 8, 2, 3);
	controlGBLayout->addWidget(microscopeButton_, 9, 10, 2, 3);
	controlGBLayout->addWidget(singleElButton_, 15, 5, 2, 3);
	controlGBLayout->addWidget(fourElButton_, 16, 11, 2, 3);
	controlGBLayout->addWidget(focusButton_, 12, 11, 2, 3);
	controlGBLayout->addWidget(lightBulb, 7, 5, 2, 2);
	controlGBLayout->addWidget(micLight_, 0, 0, 8, 1);
	controlGB->setLayout(controlGBLayout);

	window_ = new VESPERSMotorView(this);
	QVBoxLayout *windowGBLayout = new QVBoxLayout;
	windowGBLayout->addWidget(window_);
	QGroupBox *windowGB = new QGroupBox(tr("Motor Control"));
	windowGB->setMinimumSize(280, 145);
	windowGB->setLayout(windowGBLayout);

	window_->setControl(ccdControl_);

	// Setup the CCD file path signals and layout.
	ccdPathEdit_ = new QLineEdit;
	connect(ccdPathEdit_, SIGNAL(editingFinished()), this, SLOT(ccdPathEdited()));
	connect(ccdPath_, SIGNAL(valueChanged()), this, SLOT(ccdPathUpdate()));

	ccdFileEdit_ = new QLineEdit;
	connect(ccdFileEdit_, SIGNAL(editingFinished()), this, SLOT(ccdFileEdited()));
	connect(ccdFile_, SIGNAL(valueChanged()), this, SLOT(ccdFileUpdate()));

	ccdNumberEdit_ = new QLineEdit;
	connect(ccdNumberEdit_, SIGNAL(editingFinished()), this, SLOT(ccdNumberEdited()));
	connect(ccdNumber_, SIGNAL(valueChanged(int)), this, SLOT(ccdNumberUpdate(int)));

	QGroupBox *ccdGB = new QGroupBox(tr("CCD Image Path"));
	QFormLayout *ccdGBLayout = new QFormLayout;
	ccdGBLayout->addRow("Path:", ccdPathEdit_);
	ccdGBLayout->addRow("Name:", ccdFileEdit_);
	ccdGBLayout->addRow("Number:", ccdNumberEdit_);
	ccdGBLayout->setLabelAlignment(Qt::AlignRight);
	ccdGB->setLayout(ccdGBLayout);

	// Shutter layout.
	QGroupBox *shStatusGB = new QGroupBox(tr("Photon Shutter Control"));
	QHBoxLayout *shStatusLayout = new QHBoxLayout;
	shStatusLayout->addWidget(psh1);
	shStatusLayout->addWidget(psh2);
	shStatusLayout->addWidget(ssh1);
	shStatusLayout->addWidget(ssh2);
	shStatusGB->setLayout(shStatusLayout);

	// Setup the GUI with the soft limits.
	config_ = new VESPERSEndstationConfiguration;
	config_->hide();
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(loadConfiguration()));
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(loadConfiguration()));

	QToolBar *toolBar = new QToolBar(this);
	QAction *configAction = new QAction(QIcon(":/configure.png"), tr("Configure"), this);
	connect(configAction, SIGNAL(triggered()), config_, SLOT(show()));
	toolBar->addAction(configAction);
	toolBar->setMovable(false);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(ccdGB, 2, 1, 1, 6);
	layout->addWidget(shStatusGB, 0, 1, 1, 6);
	layout->addWidget(windowGB, 1, 1, 1, 6);
	layout->addWidget(controlGB, 0, 0, 4, 1);

	setLayout(layout);
	setMinimumSize(530, 465);
}

VESPERSEndstationView::~VESPERSEndstationView()
{
	delete config_;
}

void VESPERSEndstationView::updateControl()
{
	// Don't use it if it's not ready yet.
	if (window_ == 0)
		return;

	if (QString::compare("CCD", window_->control()->name()) == 0)
		window_->setControl(ccdControl_, softLimits_.value(ccdControl_).first, softLimits_.value(ccdControl_).second);
	else if (QString::compare("Microscope", window_->control()->name()) == 0)
		window_->setControl(microscopeControl_, softLimits_.value(microscopeControl_).first, softLimits_.value(microscopeControl_).second, microscopeNames_.first, microscopeNames_.second);
	else if (QString::compare("1-Element Vortex", window_->control()->name()) == 0)
		window_->setControl(singleElControl_, softLimits_.value(singleElControl_).first, softLimits_.value(singleElControl_).second);
	else if (QString::compare("4-Element Vortex", window_->control()->name()) == 0)
		window_->setControl(fourElControl_, softLimits_.value(fourElControl_).first, softLimits_.value(fourElControl_).second);
	else if (QString::compare("Focus", window_->control()->name()) == 0)
		window_->setControl(focusControl_);
}

void VESPERSEndstationView::lightBulbToggled(bool pressed)
{
	if (pressed){

		disconnect(micLight_, SIGNAL(valueChanged(int)), micLightPV_, SLOT(setValue(int)));
		disconnect(micLightPV_, SIGNAL(valueChanged()), this, SLOT(micLightUpdate()));
		micLightPV_->setValue(0);
	}
	else{

		connect(micLight_, SIGNAL(valueChanged(int)), micLightPV_, SLOT(setValue(int)));
		connect(micLightPV_, SIGNAL(valueChanged()), this, SLOT(micLightUpdate()));
		micLightPV_->setValue(micLight_->value());
	}

}

bool VESPERSEndstationView::loadConfiguration()
{
	QFile file(QDir::currentPath() + "/endstation.config");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("Endstation Configuration"),
							 tr("Cannot read file %1: \n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return false;
	}

	QTextStream in(&file);
	QStringList contents;

	while(!in.atEnd())
		contents << in.readLine();

	file.close();

	softLimits_.clear();

	softLimits_.insert(ccdControl_, qMakePair(((QString)contents.at(2)).toDouble(), ((QString)contents.at(3)).toDouble()));
	softLimits_.insert(singleElControl_, qMakePair(((QString)contents.at(6)).toDouble(), ((QString)contents.at(7)).toDouble()));
	softLimits_.insert(fourElControl_, qMakePair(((QString)contents.at(10)).toDouble(), ((QString)contents.at(11)).toDouble()));
	softLimits_.insert(microscopeControl_, qMakePair(((QString)contents.at(14)).toDouble(), ((QString)contents.at(15)).toDouble()));

	microscopeNames_ = qMakePair((QString)contents.at(16), (QString)contents.at(17));

	return true;
}

QString VESPERSEndstationView::AMPVtoString(AMProcessVariable *pv)
{
	int current;
	QString name;

	for (unsigned i = 0; i < pv->count(); i++){

		current = pv->getInt(i);
		if (current == 0)
			break;

		name += QString::fromAscii((const char *) &current);
	}

	return name;
}

void VESPERSEndstationView::StringtoAMPV(AMProcessVariable *pv, QString toConvert)
{
	int converted[256];

	for (int i = 0; i < 256; i++){

		if (i < toConvert.size())
			converted[i] = toConvert.toAscii()[i];
		else
			converted[i] = 0;
	}

	pv->setValues(converted, 256);
}

///////////////////////////////////////////////////////////////////////
// Endstation Configuration

VESPERSEndstationConfiguration::VESPERSEndstationConfiguration(QWidget *parent)
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

	QFormLayout *ccdLayout = new QFormLayout;
	ccdLayout->addRow("Low Limit", ccdLowLayout);
	ccdLayout->addRow("High Limit", ccdHighLayout);
	ccdLayout->addRow("Home Position", ccdHomeLayout);
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

	QPushButton *saveButton = new QPushButton(QIcon(":/Save.png"), tr("Save"));
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

bool VESPERSEndstationConfiguration::saveFile()
{

	QFile file(QDir::currentPath() + "endstation.config");

	if (!file.open(QFile::WriteOnly | QFile::Text)){
		QMessageBox::warning(this, tr("Endstation Configuration"),
							 tr("Cannot save file %1: \n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return false;
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

	file.close();

	hide();

	if(loadFile())
		emit configurationChanged();

	return true;
}

bool VESPERSEndstationConfiguration::loadFile()
{
	QFile file(QDir::currentPath() + "/endstation.config");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		QMessageBox::warning(this, tr("Endstation Configuration"),
							 tr("Cannot read file %1: \n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return false;
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

	return true;
}
