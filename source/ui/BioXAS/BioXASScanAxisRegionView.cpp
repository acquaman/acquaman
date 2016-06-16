#include "BioXASScanAxisRegionView.h"

BioXASScanAxisRegionView::BioXASScanAxisRegionView(AMScanAxisRegion *region, ViewMode viewMode, const AMNumber &baseValue, const QString &positionUnits, const QString &timeUnits, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	viewMode_ = Absolute;
	region_ = 0;
	baseValue_ = AMNumber(AMNumber::InvalidError);
	positionUnits_ = positionUnits;
	timeUnits_ = timeUnits;

	// Create UI elements.

	startBox_ = new QDoubleSpinBox();
	connect( startBox_, SIGNAL(valueChanged(double)), this, SLOT(onStartBoxValueChanged()) );

	stepBox_ = new QDoubleSpinBox();
	connect( stepBox_, SIGNAL(valueChanged(double)), this, SLOT(onStepBoxValueChanged()) );

	endBox_ = new QDoubleSpinBox();
	connect( endBox_, SIGNAL(valueChanged(double)), this, SLOT(onEndBoxValueChanged()) );

	timeBox_ = new QDoubleSpinBox();
	connect( timeBox_, SIGNAL(valueChanged(double)), this, SLOT(onTimeBoxValueChanged()) );

	// Create and set main layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(startBox_);
	layout->addWidget(stepBox_);
	layout->addWidget(endBox_);
	layout->addWidget(timeBox_);

	setLayout(layout);

	// Current settings.

	setRegion(region);
	setViewMode(viewMode);
	setBaseValue(baseValue);
}

BioXASScanAxisRegionView::~BioXASScanAxisRegionView()
{

}

bool BioXASScanAxisRegionView::canViewAbsolute() const
{
	return (region_ != 0);
}

bool BioXASScanAxisRegionView::canViewRelative() const
{
	return (region_ != 0 && baseValue_.isValid());
}

void BioXASScanAxisRegionView::setViewMode(ViewMode newMode)
{
	if (viewMode_ != newMode) {
		viewMode_ = newMode;
		emit viewModeChanged(viewMode_);
	}

	updateStartBox();
	updateStepBox();
	updateEndBox();
}

void BioXASScanAxisRegionView::setRegion(AMScanAxisRegion *newRegion)
{
	if (region_ != newRegion) {

		if (region_)
			disconnect( region_, 0, this, 0 );

		region_ = newRegion;

		if (region_) {
			connect( region_, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(updateStartBox()) );
			connect( region_, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(updateStepBox()) );
			connect( region_, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(updateEndBox()) );
			connect( region_, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(updateTimeBox()) );
		}

		emit regionChanged(region_);
	}

	updateStartBox();
	updateStepBox();
	updateEndBox();
	updateTimeBox();
}

void BioXASScanAxisRegionView::setBaseValue(const AMNumber &newValue)
{
	if (baseValue_ != newValue) {
		baseValue_ = newValue;
		emit baseValueChanged(baseValue_);
	}

	updateStartBox();
	updateStepBox();
	updateEndBox();
}

void BioXASScanAxisRegionView::updateStartBox()
{
	startBox_->blockSignals(true);

	startBox_->clear();
	startBox_->setAlignment(Qt::AlignCenter);
	startBox_->setPrefix("Start: ");

	startBox_->setEnabled(false);

	if (region_ && region_->regionStart().isValid() && ((viewMode_ == Absolute && canViewAbsolute()) || (viewMode_ == Relative && canViewRelative()))) {
		startBox_->setEnabled(true);
		startBox_->setRange(BIOXASSCANAXISREGIONVIEW_POSITION_MIN, BIOXASSCANAXISREGIONVIEW_POSITION_MAX);
		startBox_->setSuffix( positionUnits_.isEmpty() ? "" : QString(" %1").arg(positionUnits_));

		if (viewMode_ == Absolute)
			startBox_->setValue(double(region_->regionStart()));
		else if (viewMode_ == Relative)
			startBox_->setValue(double(getRelativeStartValue(baseValue_, region_->regionStart())));
	}

	startBox_->blockSignals(false);
}

void BioXASScanAxisRegionView::updateStepBox()
{
	stepBox_->blockSignals(true);

	stepBox_->clear();
	stepBox_->setAlignment(Qt::AlignCenter);
	stepBox_->setPrefix("Step: ");

	stepBox_->setEnabled(false);

	if (region_ && region_->regionStep().isValid()) {
		stepBox_->setEnabled(true);
		stepBox_->setRange(BIOXASSCANAXISREGIONVIEW_POSITION_MIN, BIOXASSCANAXISREGIONVIEW_POSITION_MAX);
		stepBox_->setSuffix( positionUnits_.isEmpty() ? "" : QString(" %1").arg(positionUnits_));

		stepBox_->setValue(double(region_->regionStep()));
	}

	stepBox_->blockSignals(false);
}

void BioXASScanAxisRegionView::updateEndBox()
{
	endBox_->blockSignals(true);

	endBox_->clear();
	endBox_->setAlignment(Qt::AlignCenter);
	endBox_->setPrefix("End: ");

	endBox_->setEnabled(false);

	if (region_ && region_->regionEnd().isValid() && ((viewMode_ == Absolute && canViewAbsolute()) || (viewMode_ == Relative && canViewRelative()))) {
		endBox_->setEnabled(true);
		endBox_->setRange(BIOXASSCANAXISREGIONVIEW_POSITION_MIN, BIOXASSCANAXISREGIONVIEW_POSITION_MAX);
		endBox_->setSuffix( positionUnits_.isEmpty() ? "" : QString(" %1").arg(positionUnits_));

		if (viewMode_ == Absolute)
			endBox_->setValue(double(region_->regionEnd()));
		else if (viewMode_ == Relative)
			endBox_->setValue(double(getRelativeEndValue(baseValue_, region_->regionEnd())));
	}

	endBox_->blockSignals(false);
}

void BioXASScanAxisRegionView::updateTimeBox()
{
	timeBox_->blockSignals(true);

	timeBox_->clear();
	timeBox_->setAlignment(Qt::AlignCenter);
	timeBox_->setPrefix("Time: ");

	timeBox_->setEnabled(false);

	if (region_ && region_->regionTime().isValid()) {
		timeBox_->setEnabled(true);
		timeBox_->setRange(BIOXASSCANAXISREGIONVIEW_TIME_MIN, BIOXASSCANAXISREGIONVIEW_TIME_MAX);
		timeBox_->setSuffix(timeUnits_.isEmpty() ? "" : QString(" %1").arg(timeUnits_));

		timeBox_->setValue(double(region_->regionTime()));
	}

	timeBox_->blockSignals(false);
}

void BioXASScanAxisRegionView::onStartBoxValueChanged()
{
	if (region_) {

		if (viewMode_ == Absolute)
			region_->setRegionStart(startBox_->value());
		else if (viewMode_ == Relative)
			region_->setRegionStart(getAbsoluteValue(baseValue_, startBox_->value()));
	}
}

void BioXASScanAxisRegionView::onStepBoxValueChanged()
{
	if (region_)
		region_->setRegionStep(stepBox_->value());
}

void BioXASScanAxisRegionView::onEndBoxValueChanged()
{
	if (region_) {

		if (viewMode_ == Absolute)
			region_->setRegionEnd(endBox_->value());
		else if (viewMode_ == Relative)
			region_->setRegionEnd(getAbsoluteValue(baseValue_, endBox_->value()));
	}
}

void BioXASScanAxisRegionView::onTimeBoxValueChanged()
{
	if (region_)
		region_->setRegionTime(timeBox_->value());
}

AMNumber BioXASScanAxisRegionView::getAbsoluteValue(const AMNumber &baseValue, const AMNumber &relativeValue) const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	if (baseValue.isValid() && relativeValue.isValid())
		result = AMNumber(double(baseValue_) + double(relativeValue));

	return result;
}

AMNumber BioXASScanAxisRegionView::getRelativeStartValue(const AMNumber &baseValue, const AMNumber &absoluteStartValue) const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	if (baseValue.isValid() && absoluteStartValue.isValid())
		result = AMNumber(double(baseValue) - double(absoluteStartValue));

	return result;
}

AMNumber BioXASScanAxisRegionView::getRelativeEndValue(const AMNumber &baseValue, const AMNumber &absoluteEndValue) const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	if (absoluteEndValue.isValid() && baseValue.isValid())
		result = AMNumber(double(absoluteEndValue) - double(baseValue));

	return result;
}


