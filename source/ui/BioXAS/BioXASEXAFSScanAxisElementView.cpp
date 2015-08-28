#include "BioXASEXAFSScanAxisElementView.h"
#include "util/AMEnergyToKSpaceCalculator.h"

BioXASEXAFSScanAxisElementView::BioXASEXAFSScanAxisElementView(AMScanAxisEXAFSRegion *region, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	region_ = 0;

	// Create UI elements.

	start_ = new QDoubleSpinBox;
	start_->setRange(-100000, 100000);
	start_->setSuffix(" eV");
	start_->setDecimals(2);
	start_->setAlignment(Qt::AlignCenter);

	delta_ = new QDoubleSpinBox;
	delta_->setRange(-100000, 100000);
	delta_->setSingleStep(0.5);
	delta_->setDecimals(2);
	delta_->setAlignment(Qt::AlignCenter);

	end_ = new QDoubleSpinBox;
	end_->setRange(-100000, 100000);
	end_->setDecimals(2);
	end_->setAlignment(Qt::AlignCenter);

	time_ = new QDoubleSpinBox;
	time_->setRange(-100000, 100000);
	time_->setSuffix(" s");
	time_->setDecimals(2);
	time_->setAlignment(Qt::AlignCenter);

	maximumTimeLabel_ = new QLabel("Max Time");

	maximumTime_ = new QDoubleSpinBox;
	maximumTime_->setRange(-100000, 100000);
	maximumTime_->setSuffix(" s");
	maximumTime_->setDecimals(2);
	maximumTime_->setAlignment(Qt::AlignCenter);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel("Start"), 0, Qt::AlignCenter);
	layout->addWidget(start_);
	layout->addWidget(new QLabel(QString::fromUtf8("Δ")), 0, Qt::AlignCenter);
	layout->addWidget(delta_);
	layout->addWidget(new QLabel("End"), 0, Qt::AlignCenter);
	layout->addWidget(end_);
	layout->addWidget(new QLabel("Time"), 0, Qt::AlignCenter);
	layout->addWidget(time_);
	layout->addWidget(maximumTimeLabel);
	layout->addWidget(maximumTime_);

	setLayout(layout);

	// Make connections.

	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartPositionUpdated()));
	connect(delta_, SIGNAL(editingFinished()), this, SLOT(onDeltaPositionUpdated()));
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndPositionUpdated()));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(onTimeUpdated()));
	connect(maximumTime_, SIGNAL(editingFinished()), this, SLOT(onMaximumTimeUpdated()));

	// Current settings.

	setRegion(region);
}

BioXASEXAFSScanAxisElementView::~BioXASEXAFSScanAxisElementView()
{

}

void BioXASEXAFSScanAxisElementView::setRegion(AMScanAxisEXAFSRegion *newRegion)
{
	if (region_ != newRegion) {

		if (region_) {
			disconnect( region_, 0, this, 0 );
		}

		region_ = newRegion;

		if (region_) {
			connect( region_, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStartSpinBox(AMNumber)) );
			connect( region_, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setDeltaSpinBox(AMNumber)) );
			connect( region_, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEndSpinBox(AMNumber)) );
			connect( region_, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setTimeSpinBox(AMNumber)) );
			connect( region_, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(setMaximumTimeSpinBox(AMNumber)) );
			connect( region_, SIGNAL(inKSpaceChanged(bool)), this, SLOT(update()) );
		}
	}
}

void BioXASEXAFSScanAxisElementView::clear()
{

}

void BioXASEXAFSScanAxisElementView::update()
{
	updateStartSpinBox();
	updateDeltaSpinBox();
	updateEndSpinBox();
	updateTimeSpinBox();
	updateMaximumTimeLabel();
	updateMaximumTimeSpinBox();
}

void BioXASEXAFSScanAxisElementView::refresh()
{

}

void BioXASEXAFSScanAxisElementView::setStartSpinBox(const AMNumber &value)
{
	if (double(value) != start_->value()){

		start_->setValue(double(value));

		if (region_->inKSpace())
			region_->setRegionStart(AMEnergyToKSpaceCalculator::k(region_->edgeEnergy(), start_->value()+double(region_->edgeEnergy())));

		onStartPositionUpdated();
	}
}

void BioXASEXAFSScanAxisElementView::setDeltaSpinBox(const AMNumber &value)
{
	if ((double(value) - delta_->value()) < pow(1, -1*delta_->decimals())){
		delta_->setValue(double(value));
		onDeltaPositionUpdated();
	}
}

void BioXASEXAFSScanAxisElementView::setEndSpinBox(const AMNumber &value)
{
	if ((double(value) != end_->value())){
		end_->setValue(double(value));
		onEndPositionUpdated();
	}
}

void BioXASEXAFSScanAxisElementView::setTimeSpinBox(const AMNumber &value)
{
	if (double(value) != time_->value()){
		time_->setValue(double(value));
		onTimeUpdated();
	}
}

void BioXASEXAFSScanAxisElementView::setMaximumTimeSpinBox(const AMNumber &value)
{
	if (double(value) != maximumTime_->value()){
		maximumTime_->setValue(double(value));
		onMaximumTimeUpdated();
	}
}

void BioXASEXAFSScanAxisElementView::updateStartSpinBox()
{
	if (region_) {

		// Update the value.

		double newValue;

		if (region_->inKSpace())
			newValue = double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionStart())) - double(region_->edgeEnergy());
		else
			newValue = double(region_->regionStart()) - double(region_->edgeEnergy());

		start_->blockSignals(true);
		start_->setValue(newValue);
		start_->blockSignals(false);

		// Update the enabled state.

		bool enabled = !region_->inKSpace();
		start_->setEnabled(enabled);
	}
}

void BioXASEXAFSScanAxisElementView::updateDeltaSpinBox()
{
	if (region_) {

		// Update the value.

		double newValue = double(region_->regionStep());

		delta_->blockSignals(true);
		delta_->setValue(newValue);
		delta_->blockSignals(false);

		// Update the suffix.

		QString suffix;

		if (region_->inKSpace())
			suffix = "k";
		else
			suffix = "eV";

		delta_->setSuffix(suffix);
	}
}

void BioXASEXAFSScanAxisElementView::updateEndSpinBox()
{
	if (region_) {

		// Update the value.

		double newValue;

		if (region_->inKSpace())
			newValue = double(region_->regionEnd());
		else
			newValue = double(region_->regionEnd()) - double(region_->edgeEnergy());

		end_->blockSignals(true);
		end_->setValue(newValue);
		end_->blockSignals(false);

		// Update the suffix.

		QString suffix;

		if (region_->inKSpace())
			suffix = "k";
		else
			suffix = "eV";

		end_->setSuffix(suffix);
	}
}

void BioXASEXAFSScanAxisElementView::updateTimeSpinBox()
{
	if (region_) {

		// Update the value.

		double newValue = region_->regionTime();

		time_->blockSignals(true);
		time_->setValue(newValue);
		time_->blockSignals(false);
	}
}

void BioXASEXAFSScanAxisElementView::updateMaximumTimeLabel()
{
	if (region_) {

		// Update visibility.

		bool isVisible = region_->inKSpace();

		if (isVisible)
			maximumTimeLabel_->show();
		else
			maximumTimeLabel_->hide();
	}
}

void BioXASEXAFSScanAxisElementView::updateMaximumTimeSpinBox()
{
	if (region_) {

		// Update the value.

		double newValue = region_->maximumTime();

		maximumTime_->blockSignals(true);
		maximumTime_->setValue(newValue);
		maximumTime_->blockSignals(false);

		// Update visibility.

		bool isVisible = region_->inKSpace();

		if (isVisible)
			maximumTime_->show();
		else
			maximumTime_->hide();
	}
}

void BioXASEXAFSScanAxisElementView::updateToolTip()
{
	if (region_ && region_->inKSpace()) {
		setToolTip(QString("Energy Range: %1 to %2 eV")
				   .arg(double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionStart())))
				   .arg(double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionEnd()))));
	} else {
		setToolTip("");
	}
}

void BioXASEXAFSScanAxisElementView::updateRegionStart()
{

}

void BioXASEXAFSScanAxisElementView::updateRegionStep()
{

}

void BioXASEXAFSScanAxisElementView::updateRegionEnd()
{

}

void BioXASEXAFSScanAxisElementView::updateRegionTime()
{

}

void BioXASEXAFSScanAxisElementView::updateRegionMaximumTime()
{

}
