#include "AMGenericStepScanConfigurationView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QCheckBox>
#include <QButtonGroup>

#include "util/AMDateTimeUtils.h"
#include "beamline/AMBeamline.h"

AMGenericStepScanConfigurationView::AMGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;


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
	dwellTime_->setValue(configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionTime()) : 1.0);
	dwellTime_->setMinimumWidth(100);
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	scanInformation_ = new QLabel("Scan Size:");

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);
	timeBoxLayout->addWidget(scanInformation_);

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

	if (configuration_->axisControlInfos().count() > 0){

		axisControlChoice1_->setCurrentIndex(axisControlChoice1_->findText(configuration_->axisControlInfos().at(0).name()));

		if (configuration_->axisControlInfos().count() == 2)
			axisControlChoice2_->setCurrentIndex(axisControlChoice2_->findText(configuration_->axisControlInfos().at(1).name()));
	}

	connect(axisControlChoice1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice1Changed()));
	connect(axisControlChoice2_, SIGNAL(currentIndexChanged(int)), this, SLOT(onAxisControlChoice2Changed()));

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	axisStart1_ = createPositionDoubleSpinBox("Start: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()) : -1.0, 3);
	axisStep1_ = createPositionDoubleSpinBox("Step: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()) : -1.0, 3);
	axisEnd1_ = createPositionDoubleSpinBox("End: ", "", configuration_->scanAxes().size() > 0 ? double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()) : -1.0, 3);
	axisStart2_ = createPositionDoubleSpinBox("Start: ", "", configuration_->scanAxes().size() > 1 ? double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()) : -1.0, 3);
	axisStep2_ = createPositionDoubleSpinBox("Step: ", "", configuration_->scanAxes().size() > 1 ? double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()) : -1.0, 3);
	axisEnd2_ = createPositionDoubleSpinBox("End: ", "", configuration_->scanAxes().size() > 1 ? double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) : -1.0, 3);

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
	QButtonGroup *detectorGroup = new QButtonGroup;
	detectorGroup->setExclusive(false);
	AMDetectorSet *detectors = AMBeamline::bl()->exposedDetectors();

	for (int i = 0, size = detectors->count(); i < size; i++){

		QCheckBox *checkBox = new QCheckBox(detectors->at(i)->name());
		detectorGroup->addButton(checkBox);
		detectorLayout->addWidget(checkBox);

		if (configuration_->detectorConfigurations().contains(detectors->at(i)->name()))
			checkBox->setChecked(true);
	}

	connect(detectorGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)));

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
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
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

		setStart1(-1.0);
		setStep1(-1.0);
		setEnd1(-1.0);
	}

	else{

		axisStart1_->setEnabled(true);
		axisStep1_->setEnabled(true);
		axisEnd1_->setEnabled(true);
		axisControlChoice2_->setEnabled(true);

		AMControl *control = AMBeamline::bl()->exposedControlByName(axisControlChoice1_->itemText(axisControlChoice1_->currentIndex()));
		configuration_->setControl(0, control->toInfo());
		setStart1(control->value());
		setStep1(1.0);
		setEnd1(control->value()+10);
		onStart1Changed();
		onStep1Changed();
		onEnd1Changed();
		onDwellTimeChanged();
	}

	onAxisControlChoice2Changed();

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(axisControlChoice2_->model());

	for (int i = 1; i < axisControlChoice2_->count(); i++)
		model->item(i)->setFlags(i == axisControlChoice1_->currentIndex() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onAxisControlChoice2Changed()
{
	if (axisControlChoice1_->currentIndex() == 0 || axisControlChoice2_->currentIndex() == 0){

		axisStart2_->setEnabled(false);
		axisStep2_->setEnabled(false);
		axisEnd2_->setEnabled(false);

		configuration_->removeControl(1);

		setStart2(-1.0);
		setStep2(-1.0);
		setEnd2(-1.0);
	}

	else{

		axisStart2_->setEnabled(true);
		axisStep2_->setEnabled(true);
		axisEnd2_->setEnabled(true);

		AMControl *control = AMBeamline::bl()->exposedControlByName(axisControlChoice2_->itemText(axisControlChoice2_->currentIndex()));
		configuration_->setControl(1, control->toInfo());
		setStart2(control->value());
		setStep2(1.0);
		setEnd2(control->value()+10);
		onStart2Changed();
		onStep2Changed();
		onEnd2Changed();
		onDwellTimeChanged();
	}

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(axisControlChoice1_->model());

	for (int i = 1; i < axisControlChoice1_->count(); i++)
		model->item(i)->setFlags(i == axisControlChoice2_->currentIndex() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	model->item(0)->setFlags(axisControlChoice2_->currentIndex() > 0 ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onStart1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStart(axisStart1_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onStep1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionStep(axisStep1_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onEnd1Changed()
{
	configuration_->scanAxisAt(0)->regionAt(0)->setRegionEnd(axisEnd1_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onStart2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStart(axisStart2_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onStep2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionStep(axisStep2_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onEnd2Changed()
{
	configuration_->scanAxisAt(1)->regionAt(0)->setRegionEnd(axisEnd2_->value());
	updateScanInformation();
}

void AMGenericStepScanConfigurationView::onDwellTimeChanged()
{
	if (configuration_->scanAxes().size() > 0){

		configuration_->scanAxisAt(0)->regionAt(0)->setRegionTime(dwellTime_->value());

		if (configuration_->scanAxes().size() == 2)
			configuration_->scanAxisAt(1)->regionAt(0)->setRegionTime(dwellTime_->value());
	}
}

void AMGenericStepScanConfigurationView::updateScanInformation()
{
	if (configuration_->scanAxes().size() == 1){

		double size = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
		int points = int((size)/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()));

		if ((size - (points + 0.01)*double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())) < 0)
			points += 1;

		else
			points += 2;

		scanInformation_->setText(QString("Scan Size: %1\t Points: %2")
						  .arg(QString::number(size, 'f', 1))
						  .arg(points)
						  );
	}

	else if (configuration_->scanAxes().size() == 2){

		double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
		double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

		int hPoints = int((hSize)/double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep()));
		if ((hSize - (hPoints + 0.01)*double(configuration_->scanAxisAt(0)->regionAt(0)->regionStep())) < 0)
			hPoints += 1;
		else
			hPoints += 2;

		int vPoints = int((vSize)/double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep()));
		if ((vSize - (vPoints + 0.01)*double(configuration_->scanAxisAt(1)->regionAt(0)->regionStep())) < 0)
			vPoints += 1;
		else
			vPoints += 2;

		scanInformation_->setText(QString("Scan Size: %1 x %2 \t Points: %3 x %4")
						  .arg(QString::number(hSize, 'f', 1))
						  .arg(QString::number(vSize, 'f', 1))
						  .arg(hPoints)
						  .arg(vPoints)
						  );
	}
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

void AMGenericStepScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
{
	if (button->isChecked())
		configuration_->addDetector(AMBeamline::bl()->exposedDetectorByName(button->text())->toInfo());

	else
		configuration_->removeDetector(AMBeamline::bl()->exposedDetectorByName(button->text())->toInfo());
}
