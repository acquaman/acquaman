#include "CustomizeRegionsOfInterest.h"
#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QHBoxLayout>
#include <cmath>

RegionOfInterestView::RegionOfInterestView(AMROI *roi, QWidget *parent)
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

	QLabel *value = new QLabel;
	connect(roi, SIGNAL(valueUpdate(double)), value, SLOT(setNum(double)));

	QHBoxLayout *roiLayout = new QHBoxLayout;
	roiLayout->addWidget(name_, 0, Qt::AlignCenter);
	roiLayout->addWidget(new QLabel("Low: "), 0, Qt::AlignRight);
	roiLayout->addWidget(low_);
	roiLayout->addWidget(new QLabel("High: "), 0, Qt::AlignRight);
	roiLayout->addWidget(high_);
	roiLayout->addWidget(value, 0, Qt::AlignCenter);

	setLayout(roiLayout);
	setMinimumWidth(420);
}

void RegionOfInterestView::nameUpdate(QString name)
{
	if (name.isEmpty()){

		hide();
		return;
	}

	name_->setText(name.left(name.indexOf(" ")) + " " + GeneralUtilities::addGreek(name.mid(name.indexOf(" "))));

	show();
}

CustomizeRegionsOfInterest::CustomizeRegionsOfInterest(QList<AMROI *> rois, QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout *listLayout = new QVBoxLayout;

	for (int i = 0; i < rois.size(); i++)
		listLayout->addWidget(new RegionOfInterestView(rois.at(i)));

	listLayout->addStretch();

	setLayout(listLayout);
	setMinimumSize(420, 500);
}
