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


#include "VESPERSMarCCDDetectorView.h"

#include "ui/AMTopFrame.h"

#include <QToolButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

VESPERSMarCCDDetectorView::VESPERSMarCCDDetectorView(VESPERSMarCCDDetector *detector, bool configureOnly, QWidget *parent)
	: AMDetailedOldDetectorView(configureOnly, parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool VESPERSMarCCDDetectorView::setDetector(AMOldDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	AMTopFrame *topFrame = new AMTopFrame(QString("X-Ray Diffraction - Mar CCD"));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	detector_ = static_cast<VESPERSMarCCDDetector *>(detector);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));

	isAcquiring_ = new QLabel;
	isAcquiring_->setPixmap(QIcon(":/OFF.png").pixmap(25));
	connect(detector_, SIGNAL(isAcquiringChanged(bool)), this, SLOT(onIsAcquiringChanged(bool)));

	state_ = new QLabel;
	connect(detector_, SIGNAL(stateChanged(VESPERSMarCCDDetector::State)), this, SLOT(onStateChanged(VESPERSMarCCDDetector::State)));

	acquireTime_ = new QDoubleSpinBox;
	acquireTime_->setSuffix(" s");
	acquireTime_->setDecimals(2);
	acquireTime_->setRange(0, 10000);
	acquireTime_->setAlignment(Qt::AlignCenter);
	connect(detector_, SIGNAL(acquireTimeChanged(double)), acquireTime_, SLOT(setValue(double)));
	connect(acquireTime_, SIGNAL(editingFinished()), this, SLOT(setAcquireTime()));

	QToolButton *startButton = new QToolButton;
	startButton->setIcon(QIcon(":/play_button_green.png"));
	connect(startButton, SIGNAL(clicked()), detector_, SLOT(start()));

	QToolButton *stopButton = new QToolButton;
	stopButton->setIcon(QIcon(":/red-stop-button.png"));
	connect(stopButton, SIGNAL(clicked()), detector_, SLOT(stop()));

	triggerMode_ = new QComboBox;
	triggerMode_->addItem("Free Run");
	triggerMode_->addItem("External Sync");
	connect(detector_, SIGNAL(triggerModeChanged(VESPERSMarCCDDetector::TriggerMode)), this, SLOT(onTriggerModeChanged(VESPERSMarCCDDetector::TriggerMode)));
	connect(triggerMode_, SIGNAL(currentIndexChanged(int)), this, SLOT(setTriggerMode(int)));

	imageMode_ = new QComboBox;
	imageMode_->addItem("Normal");
	imageMode_->addItem("Focus");
	connect(detector_, SIGNAL(imageModeChanged(VESPERSMarCCDDetector::ImageMode)), this, SLOT(onImageModeChanged(VESPERSMarCCDDetector::ImageMode)));
	connect(imageMode_, SIGNAL(currentIndexChanged(int)), this, SLOT(setImageMode(int)));

	QToolButton *saveButton = new QToolButton;
	saveButton->setIcon(QIcon(":/save.png"));
	connect(saveButton, SIGNAL(clicked()), detector_, SLOT(saveFile()));

	autoSaveComboBox_ = new QComboBox;
	autoSaveComboBox_->addItem("No");
	autoSaveComboBox_->addItem("Yes");
	connect(detector_, SIGNAL(autoSaveEnabledChanged(bool)), this, SLOT(onAutoSaveChanged(bool)));
	connect(autoSaveComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(setAutoSave(int)));

	// Setup the CCD file path signals and layout.
	filePathEdit_ = new QLineEdit;
	connect(filePathEdit_, SIGNAL(editingFinished()), this, SLOT(ccdPathEdited()));
	connect(detector_, SIGNAL(ccdPathChanged(QString)), filePathEdit_, SLOT(setText(QString)));

	fileNameEdit_ = new QLineEdit;
	connect(fileNameEdit_, SIGNAL(editingFinished()), this, SLOT(ccdFileEdited()));
	connect(detector_, SIGNAL(ccdNameChanged(QString)), fileNameEdit_, SLOT(setText(QString)));

	fileNumberEdit_ = new QLineEdit;
	connect(fileNumberEdit_, SIGNAL(editingFinished()), this, SLOT(ccdNumberEdited()));
	connect(detector_, SIGNAL(ccdNumberChanged(int)), this, SLOT(ccdNumberUpdate(int)));

	QGroupBox *acquisitionBox = new QGroupBox("Acquisition");

	QHBoxLayout *statusLayout = new QHBoxLayout;
	statusLayout->addWidget(isAcquiring_);
	statusLayout->addWidget(state_);
	statusLayout->addWidget(startButton);
	statusLayout->addWidget(stopButton);

	QHBoxLayout *modeLayout = new QHBoxLayout;
	modeLayout->addWidget(acquireTime_);
	modeLayout->addWidget(triggerMode_);
	modeLayout->addWidget(imageMode_);

	QVBoxLayout *acquisitionLayout = new QVBoxLayout;
	acquisitionLayout->addLayout(statusLayout);
	acquisitionLayout->addLayout(modeLayout);

	acquisitionBox->setLayout(acquisitionLayout);

	QHBoxLayout *ccdGBTopLayout = new QHBoxLayout;
	ccdGBTopLayout->addWidget(new QLabel("Autosave Image:"));
	ccdGBTopLayout->addWidget(autoSaveComboBox_);
	ccdGBTopLayout->addWidget(new QLabel("Save Current Image:"));
	ccdGBTopLayout->addWidget(saveButton);

	QGroupBox *ccdGB = new QGroupBox(tr("Image Options"));
	QFormLayout *ccdGBLayout = new QFormLayout;
	ccdGBLayout->addRow(ccdGBTopLayout);
	ccdGBLayout->addRow("Path:", filePathEdit_);
	ccdGBLayout->addRow("Name:", fileNameEdit_);
	ccdGBLayout->addRow("Number:", fileNumberEdit_);
	ccdGBLayout->setLabelAlignment(Qt::AlignRight);
	ccdGB->setLayout(ccdGBLayout);

	QVBoxLayout *detectorLayout = new QVBoxLayout;
	detectorLayout->addStretch();
	detectorLayout->addWidget(acquisitionBox);
	detectorLayout->addWidget(ccdGB);
	detectorLayout->addStretch();

	QHBoxLayout *horizontalSquishLayout = new QHBoxLayout;
	horizontalSquishLayout->addStretch();
	horizontalSquishLayout->addLayout(detectorLayout);
	horizontalSquishLayout->addStretch();

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addWidget(topFrame);
	masterLayout->addStretch();
	masterLayout->addLayout(horizontalSquishLayout);
	masterLayout->addStretch();

	setLayout(masterLayout);

	return true;
}

void VESPERSMarCCDDetectorView::onTriggerModeChanged(VESPERSMarCCDDetector::TriggerMode mode)
{
	triggerMode_->setCurrentIndex((int)mode);
}

void VESPERSMarCCDDetectorView::setTriggerMode(int newMode)
{
	if (newMode == (int)detector_->triggerMode())
		return;

	switch(newMode){

	case 0:
		detector_->setTriggerMode(VESPERSMarCCDDetector::FreeRun);
		break;

	case 1:
		detector_->setTriggerMode(VESPERSMarCCDDetector::ExtSync);
		break;
	}
}

void VESPERSMarCCDDetectorView::onImageModeChanged(VESPERSMarCCDDetector::ImageMode mode)
{
	if (mode == VESPERSMarCCDDetector::Focus)
		imageMode_->setCurrentIndex(1);
	else
		imageMode_->setCurrentIndex(0);
}

void VESPERSMarCCDDetectorView::setImageMode(int newMode)
{
	switch(newMode){

	case 0:
		if (detector_->imageMode() != VESPERSMarCCDDetector::Normal)
			detector_->setImageMode(VESPERSMarCCDDetector::Normal);

		break;

	case 1:
		if (detector_->imageMode() != VESPERSMarCCDDetector::Focus )
			detector_->setImageMode(VESPERSMarCCDDetector::Focus);

		break;
	}
}

void VESPERSMarCCDDetectorView::onStateChanged(VESPERSMarCCDDetector::State newState)
{
	switch(newState){

	case VESPERSMarCCDDetector::Idle:
		state_->setText("Idle");
		break;

	case VESPERSMarCCDDetector::Acquire:
		state_->setText("Acquire");
		break;

	case VESPERSMarCCDDetector::Readout:
		state_->setText("Readout");
		break;

	case VESPERSMarCCDDetector::Correct:
		state_->setText("Correct");
		break;

	case VESPERSMarCCDDetector::Saving:
		state_->setText("Saving");
		break;

	case VESPERSMarCCDDetector::Aborting:
		state_->setText("Aborting");
		break;

	case VESPERSMarCCDDetector::Error:
		state_->setText("Error");
		break;

	case VESPERSMarCCDDetector::Waiting:
		state_->setText("Waiting");
		break;
	}
}

void VESPERSMarCCDDetectorView::onAutoSaveChanged(bool autoSave)
{
	if (autoSave)
		autoSaveComboBox_->setCurrentIndex(1);
	else
		autoSaveComboBox_->setCurrentIndex(0);
}

void VESPERSMarCCDDetectorView::setAutoSave(int autoSave)
{
	switch(autoSave){

	case 0:
		if (detector_->autoSaveEnabled())
			detector_->setAutoSaveEnabled(false);

		break;

	case 1:
		if (!detector_->autoSaveEnabled())
			detector_->setAutoSaveEnabled(true);

		break;
	}
}
