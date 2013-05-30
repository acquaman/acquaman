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
	: AMDetailedOldDetectorView(configureOnly, parent)
{
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool VESPERSCCDDetectorView::setDetector(AMOldDetector *detector, bool configureOnly)
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

//	QPushButton *loadCCDButton = new QPushButton("Load Image");
//	connect(loadCCDButton, SIGNAL(clicked()), this, SLOT(getCCDFileNameAndLoad()));
//	connect(loadCCDButton, SIGNAL(clicked()), detector_, SLOT(loadImageFromFile()));

//	image_ = new QLabel;
//	QPixmap pixmap = QPixmap(600, 600);
//	pixmap.fill(Qt::blue);
//	image_->setPixmap(pixmap);
//	connect(detector_, SIGNAL(imageReady()), this, SLOT(displayCCDFile()));
//	connect(detector_, SIGNAL(imageReady()), this, SLOT(displayCCDFileTest()));

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
	connect(startButton, SIGNAL(clicked()), detector_, SLOT(start()));

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

void VESPERSCCDDetectorView::onTriggerModeChanged(VESPERSCCDDetector::TriggerMode mode)
{
	triggerMode_->setCurrentIndex((int)mode);
}

void VESPERSCCDDetectorView::setTriggerMode(int newMode)
{
	if (newMode == (int)detector_->triggerMode())
		return;

	switch(newMode){

	case 0:
		detector_->setTriggerMode(VESPERSCCDDetector::FreeRun);
		break;

	case 1:
		detector_->setTriggerMode(VESPERSCCDDetector::ExtSync);
		break;
	}
}

void VESPERSCCDDetectorView::onImageModeChanged(VESPERSCCDDetector::ImageMode mode)
{
	if (mode == VESPERSCCDDetector::Focus)
		imageMode_->setCurrentIndex(1);
	else
		imageMode_->setCurrentIndex(0);
}

void VESPERSCCDDetectorView::setImageMode(int newMode)
{
	switch(newMode){

	case 0:
		if (detector_->imageMode() != VESPERSCCDDetector::Normal)
			detector_->setImageMode(VESPERSCCDDetector::Normal);

		break;

	case 1:
		if (detector_->imageMode() != VESPERSCCDDetector::Focus )
			detector_->setImageMode(VESPERSCCDDetector::Focus);

		break;
	}
}

void VESPERSCCDDetectorView::onStateChanged(VESPERSCCDDetector::State newState)
{
	switch(newState){

	case VESPERSCCDDetector::Idle:
		state_->setText("Idle");
		break;

	case VESPERSCCDDetector::Acquire:
		state_->setText("Acquire");
		break;

	case VESPERSCCDDetector::Readout:
		state_->setText("Readout");
		break;

	case VESPERSCCDDetector::Correct:
		state_->setText("Correct");
		break;

	case VESPERSCCDDetector::Saving:
		state_->setText("Saving");
		break;

	case VESPERSCCDDetector::Aborting:
		state_->setText("Aborting");
		break;

	case VESPERSCCDDetector::Error:
		state_->setText("Error");
		break;

	case VESPERSCCDDetector::Waiting:
		state_->setText("Waiting");
		break;
	}
}

void VESPERSCCDDetectorView::onAutoSaveChanged(bool autoSave)
{
	if (autoSave)
		autoSaveComboBox_->setCurrentIndex(1);
	else
		autoSaveComboBox_->setCurrentIndex(0);
}

void VESPERSCCDDetectorView::setAutoSave(int autoSave)
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

void VESPERSCCDDetectorView::ccdPathEdited()
{
	if (detector_->name() == "Roper CCD" && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '\\')
		filePathEdit_->setText(filePathEdit_->text()+"\\");

	else if ((detector_->name() == "Mar CCD" || detector_->name() == "Pilatus CCD") && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '/')
		filePathEdit_->setText(filePathEdit_->text()+"/");

	detector_->setCCDPath(filePathEdit_->text());
}

//#include "MPlot/MPlotColorMap.h"
//#include <QTime>
//#include <QRgb>

//void VESPERSCCDDetectorView::displayCCDFileTest()
//{
//	QTime time1;
//	time1.start();

//	QVector<int> data = detector_->imageData();
//	QImage image = QImage(2084, 2084, QImage::Format_ARGB32);

//	QTime time;
//	time.start();

//	int maximum = data.at(0);
//	int minimum = data.at(0);

//	for (int i = 0, size = data.size(); i < size; i++){

//		if (data.at(i) < minimum)
//			minimum = data.at(i);

//		if (data.at(i) > maximum)
//			maximum = data.at(i);
//	}
//	qDebug() << "Finding min and max:" << time.restart() << "ms";
//	MPlotColorMap map = MPlotColorMap(MPlotColorMap::Jet);
//	MPlotInterval range = MPlotInterval(minimum, int(maximum*0.10));

//	int xSize = detector_->size().i();
//	int ySize = detector_->size().j();

////	for (int i = 0; i < xSize; i++){

////		int offset = i*ySize;

////		for (int j = 0; j < ySize; j++)
////			image.setPixel(i, j, map.rgbAt(data.at(j + offset), range));
////	}
//	for (int y = 0; y < ySize; y++){

//		int offset = y*xSize;
//		QRgb *line = (QRgb *)image.scanLine(y);

//		for (int x = 0; x < xSize; x++)
//			line[x] = map.rgbAt(data.at(x + offset), range);
//	}

//	qDebug() << "Time to set data in the pixels" << time.restart() << "ms";

//	QPixmap newImage;
//	newImage.convertFromImage(image);
//	qDebug() << "Converting QImage to QPixmap" << time.restart() << "ms";
//	newImage = newImage.scaled(600, 600, Qt::KeepAspectRatio);
//	qDebug() << "Scaling QPixmap" << time.restart() << "ms";
//	image_->setPixmap(newImage);

//	qDebug() << "Time to fully load and display image is" << time1.elapsed() << "ms";
//}

//void VESPERSCCDDetectorView::getCCDFileNameAndLoad()
//{

//}

//void VESPERSCCDDetectorView::displayCCDFile()
//{

//}
