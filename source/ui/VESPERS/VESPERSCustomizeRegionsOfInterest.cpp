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


#include "VESPERSCustomizeRegionsOfInterest.h"
#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QHBoxLayout>
#include <cmath>

 VESPERSRegionOfInterestView::~VESPERSRegionOfInterestView(){}
VESPERSRegionOfInterestView::VESPERSRegionOfInterestView(AMROI *roi, QWidget *parent)
	: QWidget(parent)
{
	roi_ = roi;

	connect(roi_, SIGNAL(roiHasValues()), this, SLOT(onRoiInialized()));

	name_ = new QLabel;
	connect(roi, SIGNAL(nameUpdate(QString)), this, SLOT(nameUpdate(QString)));

	low_ = new QDoubleSpinBox;
	low_->setMinimum(0);
	low_->setMaximum(30000);
	low_->setDecimals(0);
	low_->setSingleStep(roi->scale());
	low_->setSuffix(" eV");
	connect(low_, SIGNAL(valueChanged(double)), this, SLOT(setRoiLow(double)));
	connect(roi, SIGNAL(lowUpdate(int)), this, SLOT(onLowUpdate(int)));

	high_ = new QDoubleSpinBox;
	high_->setMinimum(0);
	high_->setMaximum(30000);
	high_->setDecimals(0);
	high_->setSingleStep(roi->scale());
	high_->setSuffix(" eV");
	connect(high_, SIGNAL(valueChanged(double)), this, SLOT(setRoiHigh(double)));
	connect(roi, SIGNAL(highUpdate(int)), this, SLOT(onHighUpdate(int)));

	connect(roi, SIGNAL(scalerChanged(double)), this, SLOT(onScalerChanged(double)));

	value_ = new QLabel;
	connect(roi, SIGNAL(valueUpdate(double)), value_, SLOT(setNum(double)));

	QHBoxLayout *roiLayout = new QHBoxLayout;
	roiLayout->addWidget(name_, 0, Qt::AlignCenter);
	roiLayout->addWidget(new QLabel("Low: "), 0, Qt::AlignRight);
	roiLayout->addWidget(low_);
	roiLayout->addWidget(new QLabel("High: "), 0, Qt::AlignRight);
	roiLayout->addWidget(high_);
	roiLayout->addWidget(value_, 0, Qt::AlignCenter);

	setLayout(roiLayout);
	setMinimumWidth(420);
}

void VESPERSRegionOfInterestView::nameUpdate(QString name)
{
	if (name.isEmpty()){

		hide();
		return;
	}

	name_->setText(name.left(name.indexOf(" ")) + " " + GeneralUtilities::addGreek(name.mid(name.indexOf(" "))));

	show();
}

 VESPERSCustomizeRegionsOfInterest::~VESPERSCustomizeRegionsOfInterest(){}
VESPERSCustomizeRegionsOfInterest::VESPERSCustomizeRegionsOfInterest(QList<AMROI *> rois, QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout *listLayout = new QVBoxLayout;

	for (int i = 0; i < rois.size(); i++)
		listLayout->addWidget(new VESPERSRegionOfInterestView(rois.at(i)));

	listLayout->addStretch();

	setLayout(listLayout);
	setMinimumSize(420, 500);
}
