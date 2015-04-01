#include "AMGenericStepScanConfigurationView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QCheckBox>

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
	axisStep1_ = createPositionDoubleSpinBox("Step: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()), 3);
	axisEnd1_ = createPositionDoubleSpinBox("End: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
	axisStart2_ = createPositionDoubleSpinBox("Start: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
	axisStep2_ = createPositionDoubleSpinBox("Step: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);
	axisEnd2_ = createPositionDoubleSpinBox("End: ", "", double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()), 3);

	connect(configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onScanAxisAdded(AMScanAxis*)));

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

	QScrollArea *detectorScrollArea = new QScrollArea;
	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	QVBoxLayout *detectorLayout = new QVBoxLayout;
	AMDetectorSet *detectors = AMBeamline::bl()->exposedDetectors();

	for (int i = 0, size = detectors->count(); i < size; i++){

		detectorLayout->addWidget(new QCheckBox(detectors->at(i)->name()));
	}

	detectorGroupBox->setLayout(detectorLayout);
	detectorGroupBox->setFlat(true);
	detectorScrollArea->setWidget(detectorGroupBox);
	detectorScrollArea->setFrameStyle(QFrame::NoFrame);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addWidget(detectorScrollArea);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
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

		configuration_->removeControl(0);
	}

	else{

		axisStart1_->setEnabled(true);
		axisStep1_->setEnabled(true);
		axisEnd1_->setEnabled(true);
		axisControlChoice2_->setEnabled(true);

		configuration_->setControl(0, AMBeamline::bl()->exposedControlByName(axisControlChoice1_->itemText(axisControlChoice1_->currentIndex()))->toInfo());
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

		configuration_->removeControl(1);
	}

	else{

		axisStart2_->setEnabled(true);
		axisStep2_->setEnabled(true);
		axisEnd2_->setEnabled(true);

		configuration_->setControl(1, AMBeamline::bl()->exposedControlByName(axisControlChoice2_->itemText(axisControlChoice2_->currentIndex()))->toInfo());
	}

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(axisControlChoice1_->model());

	for (int i = 1; i < axisControlChoice1_->count(); i++)
		model->item(i)->setFlags(i == axisControlChoice2_->currentIndex() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	model->item(0)->setFlags(axisControlChoice2_->currentIndex() > 0 ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void AMGenericStepScanConfigurationView::onStart1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart1_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onStep1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStep1_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onEnd1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisEnd1_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onStart2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisStart2_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onStep2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisStep2_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onEnd2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisEnd2_->value());
	updateMapInfo();
}

void AMGenericStepScanConfigurationView::onDwellTimeChanged()
{
	if (configuration_->scanAxes().size() > 0){

		configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());

		if (configuration_->scanAxes().size() == 2)
			configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
	}
}

void AMGenericStepScanConfigurationView::updateMapInfo()
{

}

void AMGenericStepScanConfigurationView::setStart1(const AMNumber &value)
{
	axisStart1_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setStep1(const AMNumber &value)
{
	axisStep1_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setEnd1(const AMNumber &value)
{
	axisEnd1_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setStart2(const AMNumber &value)
{
	axisStart2_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setStep2(const AMNumber &value)
{
	axisStep2_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setEnd2(const AMNumber &value)
{
	axisEnd2_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::setDwellTime(const AMNumber &value)
{
	dwellTime_->setValue(double(value));
}

void AMGenericStepScanConfigurationView::onScanAxisAdded(AMScanAxis *axis)
{
	if (configuration_->scanAxes().size() == 1){

		connect(axisStart1_, SIGNAL(editingFinished()), this, SLOT(onStart1Changed()));
		connect(axisStep1_, SIGNAL(editingFinished()), this, SLOT(onStep1Changed()));
		connect(axisEnd1_, SIGNAL(editingFinished()), this, SLOT(onEnd1Changed()));
		connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

		connect(axis->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setStep1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd1(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setDwellTime(AMNumber)));
	}

	else if (configuration_->scanAxes().size() == 2){

		connect(axisStart2_, SIGNAL(editingFinished()), this, SLOT(onStart2Changed()));
		connect(axisStep2_, SIGNAL(editingFinished()), this, SLOT(onStep2Changed()));
		connect(axisEnd2_, SIGNAL(editingFinished()), this, SLOT(onEnd2Changed()));

		connect(axis->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStart2(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setStep2(AMNumber)));
		connect(axis->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEnd2(AMNumber)));
	}
}
