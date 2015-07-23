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


#include "VESPERSCCDDetectorView.h"

#include "ui/AMTopFrame.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"
 VESPERSCCDDetectorView::~VESPERSCCDDetectorView(){}
VESPERSCCDDetectorView::VESPERSCCDDetectorView(VESPERSCCDDetector *detector, QWidget *parent)
	: QWidget(parent)
{
	detector_ = detector;
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));

	AMTopFrame *topFrame = new AMTopFrame(QString("X-Ray Diffraction - %1").arg(detector_->name()));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	isAcquiring_ = new QLabel;
	isAcquiring_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));
	state_ = new QLabel;
	connect(detector_, SIGNAL(acquisitionStateChanged(AMDetector::AcqusitionState)), this, SLOT(onIsAcquiringChanged()));

	acquireTime_ = new QDoubleSpinBox;
	acquireTime_->setSuffix(" s");
	acquireTime_->setDecimals(2);
	acquireTime_->setRange(0, 10000);
	acquireTime_->setAlignment(Qt::AlignCenter);
	connect(detector_, SIGNAL(acquisitionTimeChanged(double)), acquireTime_, SLOT(setValue(double)));
	connect(acquireTime_, SIGNAL(editingFinished()), this, SLOT(setAcquireTime()));

	elapsedTimeLabel_ = new QLabel("0.0 s");
	connect(&elapsedTimer_, SIGNAL(timeout()), this, SLOT(onElapsedTimerTimeout()));

	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Acquire");
	startButton_->setMaximumHeight(25);
	startButton_->setEnabled(true);
	connect(startButton_, SIGNAL(clicked()), this, SLOT(onStartClicked()));

	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	stopButton_->setMaximumHeight(25);
	stopButton_->setEnabled(false);
	connect(stopButton_, SIGNAL(clicked()), detector_, SLOT(cancelAcquisition()));

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
	statusLayout->addWidget(startButton_);
	statusLayout->addWidget(stopButton_);

	QHBoxLayout *modeLayout = new QHBoxLayout;
	modeLayout->addWidget(acquireTime_, 0, Qt::AlignCenter);
	modeLayout->addWidget(elapsedTimeLabel_, 0, Qt::AlignCenter);

	QVBoxLayout *acquisitionLayout = new QVBoxLayout;
	acquisitionLayout->addLayout(statusLayout);
	acquisitionLayout->addLayout(modeLayout);

	acquisitionBox->setLayout(acquisitionLayout);

	QGroupBox *ccdGB = new QGroupBox(tr("Image File Options"));
	QFormLayout *ccdGBLayout = new QFormLayout;
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
	masterLayout_->addLayout(horizontalSquishLayout);
	masterLayout_->addStretch();

	setLayout(masterLayout_);
}

void VESPERSCCDDetectorView::ccdPathEdited()
{
	if (detector_->name() == "Roper CCD" && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '\\')
		filePathEdit_->setText(filePathEdit_->text()+"\\");

	else if ((detector_->name() == "Mar CCD" || detector_->name() == "Pilatus CCD") && filePathEdit_->text().at(filePathEdit_->text().size()-1) != '/')
		filePathEdit_->setText(filePathEdit_->text()+"/");

	detector_->setCCDFilePath(filePathEdit_->text());
}

void VESPERSCCDDetectorView::onIsAcquiringChanged()
{
	bool acquiring = detector_->isAcquiring();

	if (acquiring){

		isAcquiring_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
		state_->setText("Acquiring");
	}

	else {

		isAcquiring_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));
		state_->setText("Idle");
		elapsedTimer_.stop();
	}

	startButton_->setDisabled(acquiring);
	stopButton_->setEnabled(acquiring);
}

void VESPERSCCDDetectorView::setAcquireTime(double time)
{
	if (time != detector_->acquisitionTime())
		detector_->setAcquisitionTime(time);
}

void VESPERSCCDDetectorView::setAcquireTime()
{
	if (acquireTime_->value() != detector_->acquisitionTime())
		detector_->setAcquisitionTime(acquireTime_->value());
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

	if (fileInfo.exists()){

		QMessageBox::StandardButton button = QMessageBox::StandardButton(QMessageBox::warning(this,
																							 "File already exists!",
																							 QString("The file \"%1\" already exists.  Do you want to overwrite?").arg(name),
																							 QMessageBox::Ok,
																							 QMessageBox::Cancel));

		if (button == QMessageBox::Ok){

			detector_->acquire();

			elapsedTimer_.start();
			elapsedTime_.restart();
		}
	}

	else{

		detector_->acquire();

		elapsedTimer_.start(100);
		elapsedTime_.restart();
	}
}

void VESPERSCCDDetectorView::onElapsedTimerTimeout()
{
	elapsedTimeLabel_->setText(QString("%1 s").arg(double(elapsedTime_.elapsed())/1000.0, 0, 'f', 1));
}
