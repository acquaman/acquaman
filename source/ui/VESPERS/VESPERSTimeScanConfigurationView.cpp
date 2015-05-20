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


#include "VESPERSTimeScanConfigurationView.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/AMDateTimeUtils.h"
#include "ui/AMTopFrame.h"

VESPERSTimeScanConfigurationView::VESPERSTimeScanConfigurationView(VESPERSTimeScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;

	AMTopFrame *frame = new AMTopFrame("VESPERS Timed Scan Configuration");

	// Dwell time.
	dwellTime_ = createDwellTimeSpinBox(configuration_->time());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_, SIGNAL(timeChanged(double)), dwellTime_, SLOT(setValue(double)));

	timePerAcquisitionBox_ = createDwellTimeSpinBox(configuration_->timePerAcquisition());
	connect(timePerAcquisitionBox_, SIGNAL(editingFinished()), this, SLOT(onTimePerAcquisitionChanged()));
	connect(configuration_, SIGNAL(timePerAcquisitionChanged(double)), timePerAcquisitionBox_, SLOT(setValue(double)));

	iterationsBox_ = new QSpinBox;
	iterationsBox_->setRange(0, 1000000);
	iterationsBox_->setPrefix("#: ");
	iterationsBox_->setAlignment(Qt::AlignCenter);
	iterationsBox_->setValue(configuration_->iterations());
	connect(iterationsBox_, SIGNAL(valueChanged(int)), this, SLOT(onIterationsChanged()));
	connect(configuration_, SIGNAL(iterationsChanged(int)), iterationsBox_, SLOT(setValue(int)));

	QFormLayout *timeLayout = new QFormLayout;
	timeLayout->addRow("Dwell Time:", dwellTime_);
	timeLayout->addRow("Time Per Acquisition:", timePerAcquisitionBox_);
	timeLayout->addRow("# of Iterations:", iterationsBox_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);

	// Using the CCD.
	ccdComboBox_ = createCCDComboBox();
	connect(ccdComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorComboBox(int)));

	// The fluorescence detector setup
	fluorescenceDetectorComboBox_  = createFluorescenceComboBox();
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetectorComboBox(int)));

	// Ion chamber selection
	itComboBox_ = createIonChamberComboBox();
	connect(itComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onItClicked(int)));
	connect(configuration_->dbObject(), SIGNAL(transmissionChoiceChanged(int)), this, SLOT(updateItComboBox(int)));

	i0ComboBox_ = createIonChamberComboBox();
	connect(i0ComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onI0Clicked(int)));
	connect(configuration_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0ComboBox(int)));

	QHBoxLayout *ionChambersLayout = new QHBoxLayout;
	ionChambersLayout->addWidget(i0ComboBox_);
	ionChambersLayout->addWidget(itComboBox_);

	// Scan name selection
	scanName_ = createScanNameView(configuration_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	// Only connecting this signal because it is the only CCD available currently.  It would need some logic for switching which CCD it was actually connected to.
	connect(VESPERSBeamline::vespers()->vespersPilatusAreaDetector(), SIGNAL(ccdPathChanged(QString)), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	QGroupBox *scanNameGroupBox = new QGroupBox("Scan Name");
	scanNameGroupBox->setLayout(scanNameLayout);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList(), configuration_->exportSpectraSources(), configuration_->exportSpectraInRows());
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));

	i0ComboBox_->setCurrentIndex((int)configuration_->incomingChoice());
	itComboBox_->setCurrentIndex((int)configuration_->transmissionChoice());
	fluorescenceDetectorComboBox_->setCurrentIndex((int)configuration_->fluorescenceDetector());
	ccdComboBox_->setCurrentIndex(ccdComboBox_->findData(int(configuration_->ccdDetector())));

	disableStandardXRDOptions();
	disableStandardI0Options();
	disableStandardItOptions();

	QFormLayout *detectorLayout = new QFormLayout;
	detectorLayout->addRow("XRF:", fluorescenceDetectorComboBox_);
	detectorLayout->addRow("XRD:", ccdComboBox_);
	detectorLayout->addRow("I0:", i0ComboBox_);
	detectorLayout->addRow("It:", itComboBox_);

	QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
	detectorGroupBox->setLayout(detectorLayout);

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(timeGroupBox, 0, 0, 2, 3);
	contentsLayout->addWidget(scanNameGroupBox, 2, 0, 1, 3);
	contentsLayout->addWidget(ccdTextBox_, 3, 0, 1, 3);
	contentsLayout->addWidget(detectorGroupBox, 0, 3, 2, 1);
	contentsLayout->addWidget(autoExportGroupBox, 2, 3, 1, 1);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addStretch();
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);

	setLayout(configViewLayout);
}

void VESPERSTimeScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
}

void VESPERSTimeScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

		QString path;

		if (configuration_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		configuration_->setCCDFileName(name);
		checkCCDFileNames(name);
	}

	if (configuration_->ccdDetector() == VESPERS::Pilatus && name.contains(" ")){

		QPalette palette = scanName_->palette();
		palette.setColor(QPalette::Base, Qt::red);
		scanName_->setPalette(palette);
	}

	else
		scanName_->setPalette(this->palette());
}

void VESPERSTimeScanConfigurationView::checkCCDFileNames(const QString &name) const
{
	QString path;

	if (configuration_->ccdDetector() == VESPERS::Roper){

		path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();
		path.replace("Y:\\", "/nas/vespers/");
		path.replace('\\', '/');
	}

	else if (configuration_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

	else if (configuration_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

	if (VESPERS::fileNameExists(path, name)){

		ccdHelpText_->show();
		ccdHelpText_->setText(QString("The scan name you have chosen conflicts with existing CCD file names.\nIf you don't a random suffix will be added to avoid name conflicts.\neg. %1").arg(VESPERS::appendUniqueIdentifier(name)));
	}

	else{

		ccdHelpText_->setText("");
		ccdHelpText_->hide();
	}
}

void VESPERSTimeScanConfigurationView::onItClicked(int index)
{
	// If the new It is at or upstream of I0, move I0.  Using id-1 is safe because Isplit can't be chosen for It.
	if (index <= i0ComboBox_->currentIndex())
		i0ComboBox_->setCurrentIndex(index-1);

	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(i0ComboBox_->model());

	for (int i = 0; i < i0ComboBox_->count(); i++)
		model->item(i)->setFlags(i < index ? (Qt::ItemIsEnabled | Qt::ItemIsSelectable) : Qt::NoItemFlags);

	configuration_->setTransmissionChoice(index);
}

void VESPERSTimeScanConfigurationView::onCCDDetectorChanged(int id)
{
	configuration_->setCCDDetector(ccdComboBox_->itemData(id).toInt());

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

		QString path;
		QString name = configuration_->ccdFileName().isEmpty() ? scanName_->text() : configuration_->ccdFileName();

		if (configuration_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		configuration_->setCCDFileName(name);
		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		checkCCDFileNames(name);
	}

	onScanNameEdited();
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);
}

void VESPERSTimeScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void VESPERSTimeScanConfigurationView::onDwellTimeChanged()
{
	configuration_->setTime(dwellTime_->value());
}

void VESPERSTimeScanConfigurationView::onTimePerAcquisitionChanged()
{
	configuration_->setTimePerAcquisition(timePerAcquisitionBox_->value());
	dwellTime_->setMaximum(timePerAcquisitionBox_->value());
}

void VESPERSTimeScanConfigurationView::onIterationsChanged()
{
	configuration_->setIterations(iterationsBox_->value());
}
