#include "AMStepScanAxisView.h"

#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

// AMStepScanAxisElementView
/////////////////////////////////////////////

AMStepScanAxisElementView::AMStepScanAxisElementView(AMScanAxisRegion *region, QWidget *parent)
	: QWidget(parent)
{
	region_ = region;

	start_ = new QDoubleSpinBox;
	start_->setRange(-100000, 100000);
	start_->setSuffix(" eV");
	start_->setDecimals(2);
	start_->setValue(region_->regionStart());
	start_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStartChanged(AMNumber)), start_, SLOT(setValue(double)));
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartPositionUpdated()));

	delta_ = new QDoubleSpinBox;
	delta_->setRange(-100000, 100000);
	delta_->setSuffix(" eV");
	delta_->setDecimals(2);
	delta_->setValue(region_->regionStep());
	delta_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStepChanged(AMNumber)), delta_, SLOT(setValue(double)));
	connect(delta_, SIGNAL(editingFinished()), this, SLOT(onDeltaPositionUpdated()));

	end_ = new QDoubleSpinBox;
	end_->setRange(-100000, 100000);
	end_->setSuffix(" eV");
	end_->setDecimals(2);
	end_->setValue(region_->regionEnd());
	end_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionEndChanged(AMNumber)), end_, SLOT(setValue(double)));
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndPositionUpdated()));

	time_ = new QDoubleSpinBox;
	time_->setRange(-100000, 100000);
	time_->setSuffix(" s");
	time_->setDecimals(2);
	time_->setValue(region_->regionTime());
	time_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionTimeChanged(AMNumber)), time_, SLOT(setValue(double)));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(onTimeUpdated()));

	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(new QLabel("Start"));
	elementViewLayout->addWidget(start_);
	elementViewLayout->addWidget(new QLabel(QString::fromUtf8("Δ")));
	elementViewLayout->addWidget(delta_);
	elementViewLayout->addWidget(new QLabel("End"));
	elementViewLayout->addWidget(end_);
	elementViewLayout->addWidget(new QLabel("Time"));
	elementViewLayout->addWidget(time_);

	setLayout(elementViewLayout);
}

void AMStepScanAxisElementView::onStartPositionUpdated()
{
	region_->setRegionStart(start_->value());
}

void AMStepScanAxisElementView::onDeltaPositionUpdated()
{
	region_->setRegionStep(delta_->value());
}

void AMStepScanAxisElementView::onEndPositionUpdated()
{
	region_->setRegionEnd(end_->value());
}

void AMStepScanAxisElementView::onTimeUpdated()
{
	region_->setRegionTime(time_->value());
}

// AMStepScanAxisView
/////////////////////////////////////////////

AMStepScanAxisView::AMStepScanAxisView(AMStepScanConfiguration *configuration, QWidget *parent)
	: QWidget(parent)
{
	configuration_ = configuration;

	QVBoxLayout *scanAxisViewLayout = new QVBoxLayout;

	foreach (AMScanAxisRegion *region, configuration_->scanAxes().first()->regions().toList()){

		QHBoxLayout *layout = new QHBoxLayout;
		layout->addWidget(new AMStepScanAxisElementView(region));
		QToolButton *deleteButton = new QToolButton;
		deleteButton->setIcon(QIcon(":22x22/list-remove-2.png"));
		deleteButton->setToolTip("Delete region");
		deleteButton->setAutoRaise(true);
		layout->addWidget(deleteButton);
		scanAxisViewLayout->addLayout(layout);
	}

	setLayout(scanAxisViewLayout);
}
