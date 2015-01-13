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


#include "IDEASXRFDetailedDetectorViewWithSave.h"

#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSSR570.h"

IDEASXRFDetailedDetectorViewWithSave::IDEASXRFDetailedDetectorViewWithSave(AMXRFDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	config_ = new IDEASXRFScanConfiguration;

	if (detector_->name() == "KETEK")
		config_->setFluorescenceDetector(IDEASXRFScanConfiguration::Ketek);

	else if (detector_->name() == "13-el Ge")
		config_->setFluorescenceDetector(IDEASXRFScanConfiguration::Ge13Element);

        scanAction_ = 0; //NULL
}

IDEASXRFDetailedDetectorViewWithSave::~IDEASXRFDetailedDetectorViewWithSave(){}

void IDEASXRFDetailedDetectorViewWithSave::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFDetailedDetectorView::buildDetectorView();

	buildScanSaveViews();

}

void IDEASXRFDetailedDetectorViewWithSave::buildScanSaveViews()
{

	scanInfoGridLayout = new QGridLayout();
	scanInfoGridLayout->setObjectName(QString::fromUtf8("scanInfoGridLayout"));
	scanInfoGridLayout->setVerticalSpacing(4);
	scanInfoGridLayout->setContentsMargins(12, -1, -1, -1);

	deadTimeCheckButton = new QPushButton("Check Dead Time");

	peakingTimeBox = new QComboBox();
	peakingTimeBox->setObjectName(QString::fromUtf8("peakingTimeBox"));
	peakingTimeBox->addItem("Setting Unknown");
	peakingTimeBox->addItem("High Rate / Low Res");
	peakingTimeBox->addItem("High Res / Low Rate");
	peakingTimeBox->addItem("Ultra Res / Slow Rate");

	connect(IDEASBeamline::ideas()->ketekPeakingTime(), SIGNAL(connected(bool)), this, SLOT(onKETEKPeakingTimeChanged()));
	connect(IDEASBeamline::ideas()->ketekPeakingTime(), SIGNAL(valueChanged(double)), this, SLOT(onKETEKPeakingTimeChanged()));

	notesEdit = new QPlainTextEdit(this);
	notesEdit->setObjectName(QString::fromUtf8("notesEdit"));
	QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
	sizePolicy2.setHorizontalStretch(4);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(notesEdit->sizePolicy().hasHeightForWidth());
	notesEdit->setSizePolicy(sizePolicy2);
	notesEdit->setStyleSheet(QString::fromUtf8( "#notesEdit {\n"
												"background-image: url(:/notepadBackground.png);\n"
												"font: bold 15px \"Marker Felt\";\n"
												"}"));
	notesEdit->setBackgroundVisible(false);

	scanNameLabel = new QLabel("Name");
	scanNameLabel->setObjectName(QString::fromUtf8("scanNameLabel"));
	scanNameLabel->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInfoGridLayout->addWidget(scanNameLabel, 0, 0, 1, 1);

	scanName = new QLineEdit(this);
	scanName->setObjectName(QString::fromUtf8("scanName"));
	scanName->setFrame(false);
	scanName->setText("XRF Scan");

	scanInfoGridLayout->addWidget(scanName, 0, 1, 1, 1);

	scanNumberLabel = new QLabel("Number");
	scanNumberLabel->setObjectName(QString::fromUtf8("scanNumberLabel"));
	scanNumberLabel->setStyleSheet(QString::fromUtf8("color: rgb(121, 121, 121);\n"
"font-weight: bold;\n"
"font-family: \"Lucida Grande\";"));

	scanInfoGridLayout->addWidget(scanNumberLabel, 1, 0, 1, 1);


	scanNumber = new QSpinBox(this);
	scanNumber->setObjectName(QString::fromUtf8("scanNumber"));
	scanNumber->setMaximumSize(QSize(16777215, 18));
	scanNumber->setFrame(false);
	scanNumber->setButtonSymbols(QAbstractSpinBox::NoButtons);
	scanNumber->setMaximum(9999);
	scanNumber->setValue(1);

	scanInfoGridLayout->addWidget(scanNumber, 1, 1, 1, 1);

	rightLayout_->addWidget(deadTimeCheckButton);

	rightLayout_->addWidget(peakingTimeBox);

	rightLayout_->addStretch();

	rightLayout_->addLayout(scanInfoGridLayout);
	rightLayout_->addWidget(notesEdit);

	saveScanButton_ = new QPushButton("Save Scan");
	saveScanButton_->setEnabled(false);

	rightLayout_->addWidget(saveScanButton_);


	connect(saveScanButton_, SIGNAL(clicked()), this, SLOT(onSaveScanButtonClicked()));
	connect(notesEdit, SIGNAL(textChanged()), this, SLOT(onNotesTextChanged()));
	connect(scanName, SIGNAL(textChanged(QString)), this, SLOT(onScanNameChanged(QString)));
	connect(scanNumber, SIGNAL(valueChanged(int)), this, SLOT(onScanNumberChanged(int)));
	connect(peakingTimeBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onPeakingTimeBoxChanged(QString)));
	connect(deadTimeCheckButton, SIGNAL(clicked()), this, SLOT(onDeadTimeCheckButtonClicked()));
	connect(acquireButton_, SIGNAL(clicked(bool)),saveScanButton_, SLOT(setEnabled(bool)));
	connect(detector_, SIGNAL(acquisitionSucceeded()),this, SLOT(onAcquisitionSucceeded()));
	connect(cancelButton_, SIGNAL(clicked()),this, SLOT(onAcquisitionSucceeded()));


}

void IDEASXRFDetailedDetectorViewWithSave::onSaveScanButtonClicked()
{

	config_->setDetectorInfo(detector_->toInfo());
	config_->setIntegrationTime(detector_->elapsedTime());

        scanAction_ = new AMScanAction(new AMScanActionInfo(config_->createCopy()));
        scanAction_->start();
	saveScanButton_->setEnabled(false);
	saveScanButton_->setText("Scan Saved");
}

void IDEASXRFDetailedDetectorViewWithSave::onNotesTextChanged()
{
	config_->setScanNotes(notesEdit->toPlainText());
}

void IDEASXRFDetailedDetectorViewWithSave::onScanNameChanged(QString name)
{
	config_->setUserScanName(name);
}

void IDEASXRFDetailedDetectorViewWithSave::onScanNumberChanged(int number)
{
	config_->setScanNumber(number);
}

void IDEASXRFDetailedDetectorViewWithSave::onPeakingTimeBoxChanged(const QString &arg1)
{

	if (arg1 == "High Rate / Low Res")
	{
	    IDEASBeamline::ideas()->ketekPeakingTime()->move(0.300);
	    IDEASBeamline::ideas()->ketekPreampGain()->move(1.2600);
	}
	else if (arg1 == "High Res / Low Rate")
	{
	    IDEASBeamline::ideas()->ketekPeakingTime()->move(2.00);
	    IDEASBeamline::ideas()->ketekPreampGain()->move(1.2375);
	}
	else if (arg1 == "Ultra Res / Slow Rate")
	{
	    IDEASBeamline::ideas()->ketekPeakingTime()->move(4.00);
	    IDEASBeamline::ideas()->ketekPreampGain()->move(1.2375);

	  }

}

void IDEASXRFDetailedDetectorViewWithSave::onAcquisitionSucceeded()
{
    saveScanButton_->setEnabled(true);
    saveScanButton_->setText("Save Scan");
    AMControlInfoList positions(IDEASBeamline::ideas()->exposedControls()->toInfoList());
    positions.remove(positions.indexOf("DwellTime"));
    positions.remove(positions.indexOf("DirectEnergy"));

    CLSSR570* I0SR570 = qobject_cast<CLSSR570*>(IDEASBeamline::ideas()->scaler()->channelAt(0)->currentAmplifier());
    AMControlInfo I0Scaler("I0Scaler", IDEASBeamline::ideas()->scaler()->channelAt(0)->voltage(), 0, 0, QString("%1 %2").arg(I0SR570->value()).arg(I0SR570->units()) , 0.1, "I_0 Scaler Value");
    positions.insert(2, I0Scaler);

    CLSSR570* SampleSR570 = qobject_cast<CLSSR570*>(IDEASBeamline::ideas()->scaler()->channelAt(1)->currentAmplifier());
    AMControlInfo SampleScaler("SampleScaler", IDEASBeamline::ideas()->scaler()->channelAt(1)->voltage(), 0, 0, QString("%1 %2").arg(SampleSR570->value()).arg(SampleSR570->units()) , 0.1, "Sample Scaler Value");
    positions.insert(3, SampleScaler);

    CLSSR570* ReferenceSR570 = qobject_cast<CLSSR570*>(IDEASBeamline::ideas()->scaler()->channelAt(2)->currentAmplifier());
    AMControlInfo ReferenceScaler("ReferenceScaler", IDEASBeamline::ideas()->scaler()->channelAt(2)->voltage(), 0, 0, QString("%1 %2").arg(ReferenceSR570->value()).arg(ReferenceSR570->units()) , 0.1, "Reference Scaler Value");
    positions.insert(4, ReferenceScaler);

    config_->setPositions(positions);
}

void IDEASXRFDetailedDetectorViewWithSave::onKETEKPeakingTimeChanged()
{

    // HACK ugly hard coded magic numbers...   Works for now.
    peakingTimeBox->blockSignals(true);

    if (IDEASBeamline::ideas()->ketekPeakingTime()->value() == 0.3)
	peakingTimeBox->setCurrentIndex(1);
    else if (IDEASBeamline::ideas()->ketekPeakingTime()->value() == 2.0)
	peakingTimeBox->setCurrentIndex(2);
    else if (IDEASBeamline::ideas()->ketekPeakingTime()->value() == 4.0)
	peakingTimeBox->setCurrentIndex(3);
    else
	peakingTimeBox->setCurrentIndex(0);

    peakingTimeBox->blockSignals(false);

}

void IDEASXRFDetailedDetectorViewWithSave::onDeadTimeCheckButtonClicked()
{
	if(detector_->isAcquiring())
		return;

	double requestedTime = detector_->acquisitionTime();

	AMListAction3 *deadTimeCheckActions = new AMListAction3(new AMListActionInfo3("Quick Deadtime Check", "Quick Deadtime Check"));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(0.1));
	deadTimeCheckActions->addSubAction(detector_->createAcquisitionAction(AMDetectorDefinitions::SingleRead));
	deadTimeCheckActions->addSubAction(detector_->createSetAcquisitionTimeAction(requestedTime));

	deadTimeCheckActions->start();
}
