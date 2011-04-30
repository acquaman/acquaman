#include "CustomizeRegionsOfInterest.h"
#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"

#include <QLabel>
#include <QHBoxLayout>
#include <cmath>

RegionOfInterestView::RegionOfInterestView(AMROI *roi, QWidget *parent)
	: QWidget(parent)
{
	QLabel *name = new QLabel(roi->name());

	QString temp = roi->name();
	temp = temp.left(temp.indexOf(" "));
	AMElement *el = AMPeriodicTable::table()->elementBySymbol(temp);

	if (el){

		int low = roi->low();
		int high = roi->high();

		for (int j = 0; j < el->emissionLines().count(); j++){

			if (el->emissionLines().at(j).first.contains("1")
					&& fabs((low+high)/2 - el->emissionLines().at(j).second.toDouble()/roi->scale()) < 1)
				name->setText(el->symbol()+" "+el->emissionLines().at(j).first);
		}
	}

	low_ = new QDoubleSpinBox;
	low_->setValue(roi->low());
	low_->setMinimum(0);
	low_->setSingleStep(roi->scale());
	low_->setSuffix(" eV");
	connect(low_, SIGNAL(valueChanged(double)), roi, SLOT(setLow(double)));
	connect(roi, SIGNAL(lowUpdate(int)), this, SLOT(lowUpdate(int)));

	high_ = new QDoubleSpinBox;
	high_->setValue(roi->high());
	high_->setMinimum(0);
	high_->setSingleStep(roi->scale());
	high_->setSuffix(" eV");
	connect(high_, SIGNAL(valueChanged(double)), roi, SLOT(setHigh(double)));
	connect(roi, SIGNAL(highUpdate(int)), this, SLOT(highUpdate(int)));

	QLabel *value = new QLabel(QString::number(roi->value()));

	QHBoxLayout *roiLayout = new QHBoxLayout;
	roiLayout->addWidget(name);
	roiLayout->addWidget(new QLabel("Low: "));
	roiLayout->addWidget(low_);
	roiLayout->addWidget(new QLabel("High: "));
	roiLayout->addWidget(high_);
	roiLayout->addWidget(value);

	setLayout(roiLayout);
}

CustomizeRegionsOfInterest::CustomizeRegionsOfInterest(QWidget *parent) :
	QWidget(parent)
{
}
