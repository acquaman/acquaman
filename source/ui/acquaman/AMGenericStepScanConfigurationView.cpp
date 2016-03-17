#include "AMGenericStepScanConfigurationView.h"

#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>

#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"
#include "util/AMDateTimeUtils.h"

AMGenericStepScanConfigurationView::AMGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;
	controls_ = 0;

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

	setControls(controls);

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

	// Set initial spinbox values in cases where we don't yet have axes:
	if(configuration_->scanAxes().count() == 0) {
		// Set defaults for axis 1 and 2
		onAxisControlChoice1Changed();
		onAxisControlChoice2Changed();
	} else if(configuration_->scanAxes().count() == 1) {
		// Set defaults just for axis 2
		onAxisControlChoice2Changed();
	}

	// Create I0 editor.

	i0ComboBox_ = new QComboBox;

	QHBoxLayout *i0Layout = new QHBoxLayout;
	i0Layout->addWidget(new QLabel("I0:"));
	i0Layout->addWidget(i0ComboBox_);

	connect(i0ComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onI0ChoiceChanged(int)));

	// Create detectors editor.

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(configuration, detectors);

	QVBoxLayout *detectorsAreaWidgetLayout = new QVBoxLayout();
	detectorsAreaWidgetLayout->addWidget(detectorsView_);
	detectorsAreaWidgetLayout->addStretch();

	QWidget *detectorsAreaWidget = new QWidget();
	detectorsAreaWidget->setLayout(detectorsAreaWidgetLayout);

	QScrollArea *detectorsArea = new QScrollArea;
	detectorsArea->setWidget(detectorsAreaWidget);
	detectorsArea->setFrameStyle(QFrame::NoFrame);

	QVBoxLayout *detectorsBoxLayout = new QVBoxLayout();
	detectorsBoxLayout->addWidget(detectorsArea);

	QGroupBox *detectorsBox = new QGroupBox("Detectors");
	detectorsBox->setLayout(detectorsBoxLayout);

	// Assemble main layout.

	QVBoxLayout *detectorsAndI0Layout = new QVBoxLayout;
	detectorsAndI0Layout->addLayout(i0Layout);
	detectorsAndI0Layout->addWidget(detectorsBox);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(positionsBox);
	layout->addWidget(scanName_);
	layout->addWidget(timeGroupBox);

	QHBoxLayout *moreLayout = new QHBoxLayout;
	moreLayout->addStretch();
	moreLayout->addLayout(layout);
	moreLayout->addLayout(detectorsAndI0Layout);
	moreLayout->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addStretch();
	configViewLayout->addLayout(moreLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);

}

AMDetectorSet* AMGenericStepScanConfigurationView::detectors() const
{
	return detectorsView_->detectors();
}

void AMGenericStepScanConfigurationView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {

		// Clear previously dislayed controls.

		axisControlChoice1_->clear();
		axisControlChoice2_->clear();

		controlNameMap_.clear();

		// Set new controls.

		controls_ = newControls;

		// Add new controls.

		axisControlChoice1_->addItem("None");
		axisControlChoice2_->addItem("None");

		for (int controlIndex = 0, controlCount = controls_->count(); controlIndex < controlCount; controlIndex++) {
			AMControl *control = controls_->at(controlIndex);

			if (control) {
				QString name = control->name();

				axisControlChoice1_->addItem(name);
				axisControlChoice2_->addItem(name);

				controlNameMap_.insert(name, control);
			}
		}

		// Update current indexes.

		if (configuration_ && configuration_->axisControlInfos().count() > 0) {

			axisControlChoice1_->setCurrentIndex(axisControlChoice1_->findText(configuration_->axisControlInfos().at(0).name()));

			if (configuration_->axisControlInfos().count() == 2)
				axisControlChoice2_->setCurrentIndex(axisControlChoice2_->findText(configuration_->axisControlInfos().at(1).name()));
		}
	}
}

//void AMGenericStepScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
//{
//	if (detectors_ != newDetectors) {

//		// Clear previously displayed detectors.

//		for (int buttonIndex = 0, buttonCount = detectorGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {
//			QAbstractButton *button = detectorGroup_->button(buttonIndex);
//			detectorLayout_->removeWidget(button);
//			detectorGroup_->removeButton(button);
//			button->deleteLater();
//		}

//		detectorButtonMap_.clear();
//		i0ComboBox_->clear();

//		// Set new detectors.

//		detectors_ = newDetectors;

//		// Add new detectors.

//		i0ComboBox_->addItem("None");

//		for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {
//			AMDetector *detector = detectors_->at(detectorIndex);

//			if (detector) {
//				QCheckBox *checkBox = new QCheckBox(detector->name());
//				detectorGroup_->addButton(checkBox);
//				detectorLayout_->addWidget(checkBox);

//				detectorButtonMap_.insert(detector, checkBox);

//				// Update current selection.

//				if (configuration_ && configuration_->detectorConfigurations().contains(detector->name())) {
//					checkBox->blockSignals(true);
//					checkBox->setChecked(true);
//					i0ComboBox_->addItem(detector->name());
//					checkBox->blockSignals(false);
//				}

//			}
//		}

//		if (configuration_ && configuration_->i0().name() != "Invalid Detector")
//			i0ComboBox_->setCurrentIndex(i0ComboBox_->findText(configuration_->i0().name()));
//	}
//}

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

		AMControl *control = controlNameMap_.value( axisControlChoice1_->itemText(axisControlChoice1_->currentIndex()), 0 );

		if (control) {
			configuration_->setControl(0, control->toInfo());
			setStart1(control->value());
			setStep1(1.0);
			setEnd1(control->value()+10);
		}

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

		AMControl *control = controlNameMap_.value(axisControlChoice2_->itemText(axisControlChoice2_->currentIndex()), 0);

		if (control) {
			configuration_->setControl(1, control->toInfo());
			setStart2(control->value());
			setStep2(1.0);
			setEnd2(control->value()+10);
		}

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
		int points = configuration_->scanAxisAt(0)->numberOfPoints();

		scanInformation_->setText(QString("Scan Size: %1\t Points: %2")
						  .arg(QString::number(size, 'f', 1))
						  .arg(points)
						  );
	}

	else if (configuration_->scanAxes().size() == 2){

		double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
		double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd())-double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

		int hPoints = configuration_->scanAxisAt(0)->numberOfPoints();
		int vPoints = configuration_->scanAxisAt(1)->numberOfPoints();

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

		connect(axisStart1_, SIGNAL(valueChanged(double)), this, SLOT(onStart1Changed()));
		connect(axisStep1_, SIGNAL(valueChanged(double)), this, SLOT(onStep1Changed()));
		connect(axisEnd1_, SIGNAL(valueChanged(double)), this, SLOT(onEnd1Changed()));
		connect(dwellTime_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeChanged()));

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

//void AMGenericStepScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
//{
//	if (button) {
//		AMDetector *detector = detectorButtonMap_.key(button, 0);

//		if (detector) {
//			if (button->isChecked()){

//				configuration_->addDetector(detector->toInfo());
//				i0ComboBox_->addItem(detector->name());
//			}

//			else {

//				configuration_->removeDetector(detector->toInfo());

//				if (i0ComboBox_->currentText() == detector->name())
//					i0ComboBox_->setCurrentIndex(i0ComboBox_->findText("None"));

//				i0ComboBox_->removeItem(i0ComboBox_->findText(detector->name()));
//			}
//		}
//	}
//}

void AMGenericStepScanConfigurationView::onI0ChoiceChanged(int index)
{
	if (i0ComboBox_->currentText() == "None")
		configuration_->setI0(AMDetectorInfo());

	else
		configuration_->setI0(configuration_->detectorConfigurations().at(configuration_->detectorConfigurations().indexOf(i0ComboBox_->itemText(index))));
}
