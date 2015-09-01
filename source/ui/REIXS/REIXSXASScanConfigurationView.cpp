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


#include "REIXSXASScanConfigurationView.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QStringBuilder>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>

#include "application/REIXS/REIXS.h"
#include "util/AMDateTimeUtils.h"

#include "ui/AMTopFrame.h"
#include "ui/dataman/AMStepScanAxisView.h"
#include "ui/dataman/AMSamplePre2013Selector.h"

REIXSXASScanConfigurationView::REIXSXASScanConfigurationView(REIXSXASScanConfiguration* config, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	if (objectName().isEmpty())
		setObjectName("REIXSXASScanConfigurationView");

	if(config)
		config_ = config;
	else
		config_ = new REIXSXASScanConfiguration(this);

	resize(828, 352);
	setupUi();
	initializeUiComponents();
	setupConnections();
}

REIXSXASScanConfigurationView::~REIXSXASScanConfigurationView()
{
}

void REIXSXASScanConfigurationView::reviewPolarizationAngleBoxEnabled()
{
	polarizationAngleBox_->setEnabled(config_->polarization() == 5 && config_->applyPolarization());
}

void REIXSXASScanConfigurationView::onRegionsChanged()
{
	config_->totalTime(true);
}

void REIXSXASScanConfigurationView::onEstimatedTimeChanged()
{
	config_->blockSignals(true);
	double time = config_->totalTime(true);
	config_->blockSignals(false);


	QString timeString = AMDateTimeUtils::convertTimeToString(time);

	estimatedTimeLabel_->setText(timeString);
	totalPointsLabel_->setText(QString::number(config_->totalPoints()));
}

void REIXSXASScanConfigurationView::setupUi()
{
	setWindowTitle("Form");

	// mono groupBox layout
	QFormLayout *monoGroupBoxFormLayout = new QFormLayout();
	monoGroupBoxFormLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	monoGroupBoxFormLayout->setLabelAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

	applyGratingBox_ = new QCheckBox("Grating");
	gratingBox_ = new QComboBox();
	monoGroupBoxFormLayout->setWidget(0, QFormLayout::LabelRole, applyGratingBox_);
	monoGroupBoxFormLayout->setWidget(0, QFormLayout::FieldRole, gratingBox_);

	applyMirrorBox_ = new QCheckBox("Mirror");
	mirrorBox_ = new QComboBox();
	monoGroupBoxFormLayout->setWidget(1, QFormLayout::LabelRole, applyMirrorBox_);
	monoGroupBoxFormLayout->setWidget(1, QFormLayout::FieldRole, mirrorBox_);

	applySlitWidthBox_ = new QCheckBox("Slit Width");
	slitWidthBox_ = createDoubleSpinBox(0, -500, 500, " um", 1, 5);
	monoGroupBoxFormLayout->setWidget(2, QFormLayout::LabelRole, applySlitWidthBox_);
	monoGroupBoxFormLayout->setWidget(2, QFormLayout::FieldRole, slitWidthBox_);

	monoGroupBoxFormLayout->setItem(3, QFormLayout::FieldRole, new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	estimatedTimeLabel_ = new QLabel("0 s");
	monoGroupBoxFormLayout->setWidget(4, QFormLayout::LabelRole, new QLabel("Estimated time:"));
	monoGroupBoxFormLayout->setWidget(4, QFormLayout::FieldRole, estimatedTimeLabel_);

	totalPointsLabel_ = new QLabel("0");
	monoGroupBoxFormLayout->setWidget(5, QFormLayout::LabelRole, new QLabel("Total points:"));
	monoGroupBoxFormLayout->setWidget(5, QFormLayout::FieldRole, totalPointsLabel_);


	// polarization groupBox layout
	QFormLayout *polarizationGroupBoxFormLayout = new QFormLayout();

	applyPolarizationBox_ = new QCheckBox("Polarization");
	polarizationBox_ = new QComboBox();
	polarizationGroupBoxFormLayout->setWidget(0, QFormLayout::LabelRole, applyPolarizationBox_);
	polarizationGroupBoxFormLayout->setWidget(0, QFormLayout::FieldRole, polarizationBox_);

	polarizationAngleBox_ = createDoubleSpinBox(0, -180, 180, " deg", 1, 10);
	polarizationGroupBoxFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Angle"));
	polarizationGroupBoxFormLayout->setWidget(1, QFormLayout::FieldRole, polarizationAngleBox_);

	// scan metal info layout
	QFormLayout *scanMetaInfoFormLayout = new QFormLayout();
	scanMetaInfoFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	nameEdit_ = new QLineEdit();
	scanMetaInfoFormLayout->setWidget(0, QFormLayout::LabelRole, new QLabel("Name"));
	scanMetaInfoFormLayout->setWidget(0, QFormLayout::FieldRole, nameEdit_);

	sampleSelector_ = new AMSamplePre2013Selector(AMDatabase::database("user"));
	scanMetaInfoFormLayout->setWidget(1, QFormLayout::LabelRole, new QLabel("Sample"));
	scanMetaInfoFormLayout->setWidget(1, QFormLayout::FieldRole, sampleSelector_);

	namedAutomaticallyBox_ = new QCheckBox("from current sample");
	scanMetaInfoFormLayout->setWidget(2, QFormLayout::LabelRole, new QLabel("Set automatically"));
	scanMetaInfoFormLayout->setWidget(2, QFormLayout::FieldRole, namedAutomaticallyBox_);


	// layout the components
	topFrame_ = new AMTopFrame("Setup XAS Scan", QIcon(":/utilities-system-monitor.png"));

	QGroupBox *monoGroupBox = new QGroupBox("Monochromator");
	monoGroupBox->setLayout(monoGroupBoxFormLayout);

	QGroupBox *polarizationGroupBox = new QGroupBox("Polarization");
	polarizationGroupBox->setLayout(polarizationGroupBoxFormLayout);

	QGroupBox *scanMetaInfoGroupBox = new QGroupBox("Scan Meta-Information");
	scanMetaInfoGroupBox->setLayout(scanMetaInfoFormLayout);

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	horizontalLayout->addWidget(monoGroupBox);
	horizontalLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	horizontalLayout->addWidget(polarizationGroupBox);
	horizontalLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	horizontalLayout->addWidget(scanMetaInfoGroupBox);

	QVBoxLayout *innerVLayout = new QVBoxLayout();
	innerVLayout->setSpacing(0);
	innerVLayout->setContentsMargins(12, 12, 12, 12);
	innerVLayout->addLayout(horizontalLayout);
	innerVLayout->insertWidget(0, new AMStepScanAxisView("Region Configuration", config_));
	innerVLayout->addStretch();

	QVBoxLayout *outerVLayout = new QVBoxLayout();
	outerVLayout->insertWidget(0, topFrame_);
	outerVLayout->setContentsMargins(0, 0, 0, 0);
	outerVLayout->addLayout(innerVLayout);
	setLayout(outerVLayout);

}

void REIXSXASScanConfigurationView::initializeUiComponents()
{
	// initialze the comboBox list
	gratingBox_->clear();
	gratingBox_->insertItems(0, QStringList() << "Ni LEG" << "Au LEG" << "Au HEG" );

	mirrorBox_->clear();
	mirrorBox_->insertItems(0, QStringList() << "Nickel" << "Carbon" << "Silicon" << "Gold" );

	polarizationBox_->clear();
	polarizationBox_->insertItems(0, QStringList()
								  << "Circular Left"
								  << "Circular Right"
								  << "Linear Horizontal"
								  << "Linear Vertical -"
								  << "Linear Vertical +"
								  << "Linear Inclined" );

	// Initialize widgets from config_
	applyGratingBox_->setChecked(false);
	applyGratingBox_->setDisabled(true);
	gratingBox_->setCurrentIndex(config_->monoGrating());
	gratingBox_->setEnabled(config_->applyMonoGrating());
	gratingBox_->setDisabled(true);

	applyMirrorBox_->setChecked(config_->applyMonoMirror());
	applyMirrorBox_->setDisabled(true);
	mirrorBox_->setCurrentIndex(false);
	mirrorBox_->setEnabled(config_->applyMonoMirror());
	mirrorBox_->setDisabled(true);


	applySlitWidthBox_->setChecked(config_->applySlitWidth());
	slitWidthBox_->setValue(config_->slitWidth());
	slitWidthBox_->setEnabled(config_->applySlitWidth());

	applyPolarizationBox_->setChecked(config_->applyPolarization());
	polarizationBox_->setCurrentIndex(config_->polarization());
	polarizationAngleBox_->setValue(config_->polarizationAngle());
	polarizationBox_->setEnabled(config_->applyPolarization());
	polarizationAngleBox_->setEnabled(config_->applyPolarization() && config_->polarization() == 5);

	nameEdit_->setText(config_->userScanName());
	sampleSelector_->setCurrentSample(config_->sampleId());

	namedAutomaticallyBox_->setChecked(config_->namedAutomatically());
	nameEdit_->setEnabled(!config_->namedAutomatically());
	sampleSelector_->setEnabled(!config_->namedAutomatically());

	onEstimatedTimeChanged();
}

void REIXSXASScanConfigurationView::setupConnections()
{
	// Make connections: from widgets to scan configuration.
	connect(applyGratingBox_, SIGNAL(clicked(bool)), config_, SLOT(setApplyMonoGrating(bool)));
	connect(applyMirrorBox_, SIGNAL(clicked(bool)), config_, SLOT(setApplyMonoMirror(bool)));
	connect(applySlitWidthBox_, SIGNAL(clicked(bool)), config_, SLOT(setApplySlitWidth(bool)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), config_, SLOT(setApplyPolarization(bool)));
	connect(namedAutomaticallyBox_, SIGNAL(clicked(bool)), config_, SLOT(setNamedAutomatically(bool)));

	connect(gratingBox_, SIGNAL(currentIndexChanged(int)), config_, SLOT(setMonoGrating(int)));
	connect(mirrorBox_, SIGNAL(currentIndexChanged(int)), config_, SLOT(setMonoMirror(int)));
	connect(slitWidthBox_, SIGNAL(valueChanged(double)), config_, SLOT(setSlitWidth(double)));
	connect(polarizationBox_, SIGNAL(currentIndexChanged(int)), config_, SLOT(setPolarization(int)));
	connect(polarizationAngleBox_, SIGNAL(valueChanged(double)), config_, SLOT(setPolarizationAngle(double)));
	connect(nameEdit_, SIGNAL(textEdited(QString)), config_, SLOT(setUserScanName(QString)));
	connect(sampleSelector_, SIGNAL(currentSampleChanged(int)), config_, SLOT(setSampleId(int)));
	connect(namedAutomaticallyBox_, SIGNAL(clicked(bool)), config_, SLOT(setNamedAutomatically(bool)));

	// Make connections: from widgets to enable/disable other widgets
	connect(applyGratingBox_, SIGNAL(clicked(bool)), gratingBox_, SLOT(setEnabled(bool)));
	connect(applyMirrorBox_, SIGNAL(clicked(bool)), mirrorBox_, SLOT(setEnabled(bool)));
	connect(applySlitWidthBox_, SIGNAL(clicked(bool)), slitWidthBox_, SLOT(setEnabled(bool)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), polarizationBox_, SLOT(setEnabled(bool)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(polarizationBox_, SIGNAL(activated(int)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(namedAutomaticallyBox_, SIGNAL(clicked(bool)), nameEdit_, SLOT(setDisabled(bool)));
	connect(namedAutomaticallyBox_, SIGNAL(clicked(bool)), sampleSelector_, SLOT(setDisabled(bool)));

	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(modifiedChanged(bool)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(onEstimatedTimeChanged()));
}

QDoubleSpinBox *REIXSXASScanConfigurationView::createDoubleSpinBox(double value, double min, double max, QString suffix, int decimals, double step)
{
	QDoubleSpinBox* spinBox = new QDoubleSpinBox();
	spinBox->setValue(value);
	spinBox->setMinimum(min);
	spinBox->setMaximum(max);
	spinBox->setSuffix(suffix);
	spinBox->setDecimals(decimals);
	spinBox->setSingleStep(step);

	spinBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

	return spinBox;
}
