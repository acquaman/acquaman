/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSCCDDetectorView.h"

#include "ui/AMTopFrame.h"

#include <QToolButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"
VESPERSCCDDetectorView::VESPERSCCDDetectorView(VESPERSCCDDetector *detector, bool configureOnly, QWidget *parent)
	: QWidget(parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool VESPERSCCDDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	AMTopFrame *topFrame = new AMTopFrame(QString("X-Ray Diffraction - Roper CCD"));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	detector_ = static_cast<VESPERSCCDDetector *>(detector);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));

	isAcquiring_ = new QLabel;
	isAcquiring_->setPixmap(QIcon(":/OFF.png").pixmap(25));
	connect(detector_, SIGNAL(isAcquiringChanged(bool)), this, SLOT(onIsAcquiringChanged(bool)));

	state_ = new QLabel;
	connect(detector_, SIGNAL(stateChanged(VESPERSCCDDetector::State)), this, SLOT(onStateChanged(VESPERSCCDDetector::State)));

	acquireTime_ = new QDoubleSpinBox;
	acquireTime_->setSuffix(" s");
	acquireTime_->setDecimals(2);
	acquireTime_->setRange(0, 10000);
	acquireTime_->setAlignment(Qt::AlignCenter);
	connect(detector_, SIGNAL(acquireTimeChanged(double)), acquireTime_, SLOT(setValue(double)));
	connect(acquireTime_, SIGNAL(editingFinished()), this, SLOT(setAcquireTime()));

	QToolButton *startButton = new QToolButton;
	startButton->setIcon(QIcon(":/play_button_green.png"));
	connect(startButton, SIGNAL(clicked()), this, SLOT(onStartClicked()));

	QToolButton *stopButton = new QToolButton;
	stopButton->setIcon(QIcon(":/red-stop-button.png"));
	connect(stopButton, SIGNAL(clicked()), detector_, SLOT(stop()));

	triggerMode_ = new QComboBox;
	triggerMode_->addItem("Free Run");
	triggerMode_->addItem("External Sync");
	connect(detector_, SIGNAL(triggerModeChanged(VESPERSCCDDetector::TriggerMode)), this, SLOT(onTriggerModeChanged(VESPERSCCDDetector::TriggerMode)));
	connect(triggerMode_, SIGNAL(currentIndexChanged(int)), this, SLOT(setTriggerMode(int)));

	imageMode_ = new QComboBox;
	imageMode_->addItem("Normal");
	imageMode_->addItem("Focus");
	connect(detector_, SIGNAL(imageModeChanged(VESPERSCCDDetector::ImageMode)), this, SLOT(onImageModeChanged(VESPERSCCDDetector::ImageMode)));
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

	QGroupBox *ccdGB = new QGroupBox(tr("Image File Options"));
	QFormLayout *ccdGBLayout = new QFormLayout;
	ccdGBLayout->addRow(ccdGBTopLayout);
	ccdGBLayout->addRow("Path:", filePathEdit_);
	ccdGBLayout->addRow("Name:", fileNameEdit_);
	ccdGBLayout->addRow("Number:", fileNumberEdit_);
	ccdGBLayout->setLabelAlignment(Qt::AlignRight);
	ccdGB->setLayout(ccdGBLayout);

	QVBoxLayout *acquisitionAndTemperatureLayout = new QVBoxLayout;
	acquisitionAndTemperatureLayout->addWidget(acquisitionBox);
	acquisitionAndTemperatureLayout->addStretch();

	detectorLayout_ = new QVBoxLayout;
	detectorLayout_->addStretch();
	detectorLayout_->addLayout(acquisitionAndTemperatureLayout);
	detectorLayout_->addWidget(ccdGB);
	detectorLayout_->addStretch();

	QHBoxLayout *horizontalSquishLayout = new QHBoxLayout;
	horizontalSquishLayout->addStretch();
	horizontalSquishLayout->addLayout(detectorLayout_);
	horizontalSquishLayout->addStretch();

	masterLayout_ = new QVBoxLayout;
	masterLayout_->addWidget(topFrame);
	masterLayout_->addStretch();
//	masterLayout->addWidget(image_, 0, Qt::AlignCenter);
//	masterLayout->addWidget(loadCCDButton);
//	masterLayout->addLayout(horizontalSquishLayout);
	masterLayout_->addLayout(horizontalSquishLayout);
	masterLayout_->addStretch();

	setLayout(masterLayout_);

	return true;
}

void VESPERSCCDDetectorView::ccdPathEdited()
{
	if (detector_->name() == "Roper CCD" && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '\\')
		filePathEdit_->setText(filePathEdit_->text()+"\\");

	else if ((detector_->name() == "Mar CCD" || detector_->name() == "Pilatus CCD") && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '/')
		filePathEdit_->setText(filePathEdit_->text()+"/");

	detector_->setCCDFilePath(filePathEdit_->text());
}

void VESPERSCCDDetectorView::onIsAcquiringChanged(bool isAcquiring)
{
	isAcquiring_->setPixmap(QIcon(isAcquiring ? ":/ON.png" : ":/OFF.png").pixmap(25));
}

void VESPERSCCDDetectorView::setAcquireTime(double time)
{
//	if (time != detector_->acquireTime())
//		detector_->setAcquireTime(time);
}

void VESPERSCCDDetectorView::setAcquireTime()
{
//	if (acquireTime_->value() != detector_->acquireTime())
//		detector_->setAcquireTime(acquireTime_->value());
}

void VESPERSCCDDetectorView::ccdFileEdited()
{
	detector_->setCCDFileBaseName(fileNameEdit_->text());
}

void VESPERSCCDDetectorView::ccdNumberEdited()
{
	detector_->setCCDNumber(fileNumberEdit_->text().toInt());
}

void VESPERSCCDDetectorView::ccdNumberUpdate(int val)
{
	fileNumberEdit_->setText(QString::number(val));
}

void VESPERSCCDDetectorView::onStartClicked()
{
	QString fullPath = detector_->ccdFilePath();
	QString name = detector_->ccdFileBaseName();

	if (detector_->name().contains("Roper"))
		name.append(QString("_%1.spe").arg(detector_->ccdFileNumber()));

	else if (detector_->name().contains("Mar"))
		name.append(QString("_%1.tif").arg(detector_->ccdFileNumber()));

	else if (detector_->name().contains("Pilatus")){

		fullPath = fullPath.replace("/ramdisk", "/nas/pilatus/500_500");
		name.append(QString("-%1.tif").arg(detector_->ccdFileNumber()));
	}

	fullPath.append(name);
	QFileInfo fileInfo = QFileInfo(fullPath);

//	if (fileInfo.exists()){

//		QMessageBox::StandardButton button = QMessageBox::StandardButton(QMessageBox::warning(this,
//																							 "File already exists!",
//																							 QString("The file \"%1\" already exists.  Do you want to overwrite?").arg(name),
//																							 QMessageBox::Ok,
//																							 QMessageBox::Cancel));

//		if (button == QMessageBox::Ok)
//			detector_->start();
//	}

//	else
//		detector_->start();
}
