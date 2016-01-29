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


#include "VESPERSPilatusCCDDetectorView.h"

 VESPERSPilatusCCDDetectorView::~VESPERSPilatusCCDDetectorView(){}
VESPERSPilatusCCDDetectorView::VESPERSPilatusCCDDetectorView(VESPERSPilatusCCDDetector *detector, QWidget *parent)
	: VESPERSCCDDetectorView(detector, parent)
{
	QGroupBox *diskUsageBox = new QGroupBox("Aurora File System");
	diskUsageBar_ = new QProgressBar;
	diskUsageBar_->setRange(0, 1);
	diskUsageBar_->setValue(0);
	diskUsageBar_->setTextVisible(false);
	spaceLabel_ = new QLabel;
	status_ = new QLabel;
	status_->setPixmap(QIcon(":/32x32/yellowLEDOn.png").pixmap(20));

	QHBoxLayout *barAndLabelLayout = new QHBoxLayout;
	barAndLabelLayout->addWidget(new QLabel("Disk Usage:"));
	barAndLabelLayout->addWidget(diskUsageBar_);
	barAndLabelLayout->addWidget(spaceLabel_);
	barAndLabelLayout->addWidget(status_);
	diskUsageBox->setLayout(barAndLabelLayout);

	connect(detector, SIGNAL(fileSystemInfoUpdated()), this, SLOT(onFileSystemInfoUpdate()));

	detectorLayout_->insertWidget(1, diskUsageBox);
}

void VESPERSPilatusCCDDetectorView::onFileSystemInfoUpdate()
{
	VESPERSPilatusCCDDetector *detector = (VESPERSPilatusCCDDetector *)detector_;
	diskUsageBar_->setRange(0, detector->totalAuroraSize());
	diskUsageBar_->setValue(detector->occupiedAuroraSize());
	spaceLabel_->setText(QString("%1 TB/%2 TB").arg(detector->occupiedAuroraSize(), 0, 'f', 2).arg(detector->totalAuroraSize(), 0, 'f', 2));

	double diskUsage = detector->occupiedAuroraSize()/detector->totalAuroraSize();

	if (diskUsage < 0.6)
		status_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(20));

	else if (diskUsage >= 0.6 && diskUsage < 0.8)
		status_->setPixmap(QIcon(":/32x32/yellowLEDOn.png").pixmap(20));

	else
		status_->setPixmap(QIcon(":/32x32/redLEDOn.png").pixmap(20));
}
