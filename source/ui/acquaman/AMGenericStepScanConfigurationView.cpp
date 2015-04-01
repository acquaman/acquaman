#include "AMGenericStepScanConfigurationView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "ui/AMTopFrame.h"
#include "beamline/AMBeamline.h"

AMGenericStepScanConfigurationView::AMGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	AMTopFrame *frame = new AMTopFrame("BioXAS Commissioning Tool");

	scanName_ = new QLineEdit;
	scanName_->setText(configuration_->name());
	scanName_->setAlignment(Qt::AlignCenter);
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	// Dwell time.
	dwellTime_ = new QDoubleSpinBox;
	dwellTime_->setRange(0, 1000000);
	dwellTime_->setSuffix(" s");
	dwellTime_->setAlignment(Qt::AlignCenter);
	dwellTime_->setDecimals(1);
	dwellTime_->setValue(double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()));
	dwellTime_->setMinimumWidth(100);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_->scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	axisControlChoice1_ = new QComboBox;
	axisControlChoice2_ = new QComboBox;
	AMControlSet *exposedControls = AMBeamline::bl()->exposedControls();

	axisControlChoice1_->addItem("None");
	axisControlChoice2_->addItem("None");

	for (int i = 0, size = exposedControls->count(); i < size; i++){

		axisControlChoice1_->addItem(exposedControls->at(i)->name());
		axisControlChoice2_->addItem(exposedControls->at(i)->name());
	}

	connect(axisControlChoice1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice1Changed()));
	connect(axisControlChoice2_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice2Changed()));

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	axisStart1_ = createPositionDoubleSpinBox("Start: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisStart1_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(), axisStart1_, SLOT(setValue(double)));

	axisStep1_ = createPositionDoubleSpinBox("Step: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisStep1_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(startChanged(double)), axisStep1_, SLOT(setValue(double)));

	axisEnd1_ = createPositionDoubleSpinBox("End: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisEnd1_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(startChanged(double)), axisEnd1_, SLOT(setValue(double)));

	axisStart2_ = createPositionDoubleSpinBox("Start: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisStart2_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(startChanged(double)), axisStart2_, SLOT(setValue(double)));

	axisStep2_ = createPositionDoubleSpinBox("Step: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisStep2_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(startChanged(double)), axisStep2_, SLOT(setValue(double)));

	axisEnd2_ = createPositionDoubleSpinBox("End: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
//	connect(axisEnd2_, SIGNAL(editingFinished()), this, SLOT(onStartChanged()));
//	connect(configuration_, SIGNAL(startChanged(double)), axisEnd2_, SLOT(setValue(double)));

	QHBoxLayout *axis1Layout = new QHBoxLayout;
	axis1Layout->addWidget(axisStart1_);
	axis1Layout->addWidget(axisStep1_);
	axis1Layout->addWidget(axisEnd1_);
	axis1Layout->addWidget(axisControlChoice1_);

	QHBoxLayout *axis2Layout = new QHBoxLayout;
	axis2Layout->addWidget(axisStart2_);
	axis2Layout->addWidget(axisStep2_);
	axis2Layout->addWidget(axisEnd2_);
	axis2Layout->addWidget(axisControlChoice2_);

	QVBoxLayout *positionLayout = new QVBoxLayout;
	positionLayout->addLayout(axis1Layout);
	positionLayout->addLayout(axis2Layout);

	positionsBox->setLayout(positionLayout);

	onAxisControlChoice1Changed();
	onAxisControlChoice2Changed();

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(layout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}

void AMGenericStepScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);
}

void AMGenericStepScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(configuration_->totalTime()));
}

QString AMGenericStepScanConfigurationView::convertTimeToString(double time)
{
	QString timeString;

	int days = int(time/3600.0/24.0);

	if (days > 0){

		time -= days*3600.0*24;
		timeString += QString::number(days) + "d:";
	}

	int hours = int(time/3600.0);

	if (hours > 0){

		time -= hours*3600;
		timeString += QString::number(hours) + "h:";
	}

	int minutes = int(time/60.0);

	if (minutes > 0){

		time -= minutes*60;
		timeString += QString::number(minutes) + "m:";
	}

	int seconds = ((int)time)%60;
	timeString += QString::number(seconds) + "s";

	return timeString;
}

void AMGenericStepScanConfigurationView::onDwellTimeChanged()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());
}

void AMGenericStepScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

QDoubleSpinBox * AMGenericStepScanConfigurationView::createPositionDoubleSpinBox(const QString &prefix, const QString &suffix, double value, int decimals)
{
	QDoubleSpinBox *box = new QDoubleSpinBox;
	box->setPrefix(prefix);
	box->setRange(-100000, 100000);
	box->setSuffix(suffix);
	box->setDecimals(decimals);
	box->setValue(value);
	box->setAlignment(Qt::AlignCenter);

	return box;
}

void AMGenericStepScanConfigurationView::onAxisControlChoice1Changed()
{
	if (axisControlChoice1_->currentIndex() == 0){

		axisStart1_->setEnabled(false);
		axisStep1_->setEnabled(false);
		axisEnd1_->setEnabled(false);
		axisControlChoice2_->setEnabled(false);
	}

	else{

		axisStart1_->setEnabled(true);
		axisStep1_->setEnabled(true);
		axisEnd1_->setEnabled(true);
		axisControlChoice2_->setEnabled(true);
	}

	onAxisControlChoice2Changed();

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(axisControlChoice2_->model());

	for (int i = 1; i < axisControlChoice2_->count(); i++)
		model->item(i)->setFlags(i == axisControlChoice1_->currentIndex() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void AMGenericStepScanConfigurationView::onAxisControlChoice2Changed()
{
	if (axisControlChoice1_->currentIndex() == 0 || axisControlChoice2_->currentIndex() == 0){

		axisStart2_->setEnabled(false);
		axisStep2_->setEnabled(false);
		axisEnd2_->setEnabled(false);
	}

	else{

		axisStart2_->setEnabled(true);
		axisStep2_->setEnabled(true);
		axisEnd2_->setEnabled(true);
	}


	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(axisControlChoice1_->model());

	for (int i = 1; i < axisControlChoice1_->count(); i++)
		model->item(i)->setFlags(i == axisControlChoice2_->currentIndex() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

