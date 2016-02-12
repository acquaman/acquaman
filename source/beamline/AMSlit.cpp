#include "AMSlit.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"
#include "beamline/AMSlitGap.h"
#include "beamline/AMSlitCenter.h"
#include "util/AMErrorMonitor.h"

AMSlit::AMSlit(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	// Initialize class variables.

	firstBlade_ = 0;
	secondBlade_ = 0;

	// Initialize gap control.

	gap_ = new AMSlitGap(QString("%1%2").arg(name).arg("Gap"), this);
	addChildControl(gap_);

	connect( gap_, SIGNAL(valueChanged(double)), this, SIGNAL(gapValueChanged(double)) );

	// Initialize center control.

	center_ = new AMSlitCenter(QString("%1%2").arg(name).arg("Center"), this);
	addChildControl(center_);

	connect( center_, SIGNAL(valueChanged(double)), this, SIGNAL(centerValueChanged(double)) );
}

AMSlit::~AMSlit()
{

}

bool AMSlit::canOpen() const
{
	return (firstBladeCanOpen() && secondBladeCanOpen());
}

bool AMSlit::canClose() const
{
	return (firstBladeCanClose() && secondBladeCanClose());
}

double AMSlit::firstBladeValue() const
{
	double result = -1;

	if (firstBlade_ && firstBlade_->canMeasure())
		result = firstBlade_->value();

	return result;
}

double AMSlit::secondBladeValue() const
{
	double result = -1;

	if (secondBlade_ && secondBlade_->canMeasure())
		result = secondBlade_->value();

	return result;
}

double AMSlit::gapValue() const
{
	double result = -1;

	if (gap_ && gap_->canMeasure())
		result = gap_->value();

	return result;
}

double AMSlit::centerValue() const
{
	double result = -1;

	if (center_ && center_->canMeasure())
		result = center_->value();

	return result;
}

AMAction3* AMSlit::createOpenAction()
{
	AMAction3 *result = 0;

	if (canOpen()) {
		AMListAction3 *openAction = new AMListAction3(new AMListActionInfo3(QString("Opening slit %1").arg(name()), QString("Opening slit %1").arg(name())), AMListAction3::Parallel);
		openAction->addSubAction(AMActionSupport::buildControlMoveAction(firstBlade_, bladeOpenValueMap_.value(firstBlade_)));
		openAction->addSubAction(AMActionSupport::buildControlMoveAction(secondBlade_, bladeOpenValueMap_.value(secondBlade_)));

		result = openAction;
	}

	return result;
}

AMAction3* AMSlit::createCloseAction()
{
	AMAction3 *result = 0;

	if (canClose()) {
		AMListAction3 *closeAction = new AMListAction3(new AMListActionInfo3(QString("Closing slit %1").arg(name()), QString("Closing slit %1").arg(name())), AMListAction3::Parallel);
		closeAction->addSubAction(AMActionSupport::buildControlMoveAction(firstBlade_, bladeClosedValueMap_.value(firstBlade_)));
		closeAction->addSubAction(AMActionSupport::buildControlMoveAction(secondBlade_, bladeClosedValueMap_.value(secondBlade_)));

		result = closeAction;
	}

	return result;
}

void AMSlit::setFirstBlade(AMControl *newControl, BladeOrientation orientation)
{
	if (firstBlade_ != newControl) {

		if (firstBlade_)
			removeFirstBlade();

		firstBlade_ = newControl;

		if (firstBlade_) {
			addChildControl(firstBlade_);
			bladeOrientationMap_.insert(firstBlade_, orientation);
		}

		updateGap();
		updateCenter();

		emit firstBladeChanged(firstBlade_);
	}
}

void AMSlit::removeFirstBlade()
{
	if (firstBlade_) {
		removeChildControl(firstBlade_);
		bladeOrientationMap_.remove(firstBlade_);
	}
}

void AMSlit::setSecondBlade(AMControl *newControl, BladeOrientation orientation)
{
	if (secondBlade_ != newControl) {

		if (secondBlade_)
			removeSecondBlade();

		secondBlade_ = newControl;

		if (secondBlade_) {
			addChildControl(secondBlade_);
			bladeOrientationMap_.insert(secondBlade_, orientation);
		}

		updateGap();
		updateCenter();

		emit secondBladeChanged(secondBlade_);
	}
}

void AMSlit::removeSecondBlade()
{
	if (secondBlade_) {
		removeChildControl(secondBlade_);
		bladeOrientationMap_.remove(secondBlade_);
	}
}

void AMSlit::setOpenValues(double firstBladeOpenValue, double secondBladeOpenValue)
{
	if (firstBlade_)
		bladeOpenValueMap_.insert(firstBlade_, firstBladeOpenValue);

	if (secondBlade_)
		bladeOpenValueMap_.insert(secondBlade_, secondBladeOpenValue);
}

void AMSlit::removeOpenValues()
{
	bladeOpenValueMap_.clear();
}

void AMSlit::setClosedValues(double firstBladeClosedValue, double secondBladeClosedValue)
{
	if (firstBlade_)
		bladeClosedValueMap_.insert(firstBlade_, firstBladeClosedValue);

	if (secondBlade_)
		bladeClosedValueMap_.insert(secondBlade_, secondBladeClosedValue);
}

void AMSlit::removeClosedValues()
{
	bladeClosedValueMap_.clear();
}

void AMSlit::open()
{
	AMAction3 *openAction = createOpenAction();

	if (canOpen() && openAction) {
		connect( openAction, SIGNAL(cancelled()), openAction, SLOT(deleteLater()) );
		connect( openAction, SIGNAL(failed()), openAction, SLOT(deleteLater()) );
		connect( openAction, SIGNAL(succeeded()), openAction, SLOT(deleteLater()) );

		openAction->start();

	} else {
		AMErrorMon::alert(this, AMSLIT_CANNOT_OPEN, "Cannot open the slit, opening for this slit is not supported.");
	}
}

void AMSlit::close()
{
	AMAction3 *closeAction = createCloseAction();

	if (canClose() && closeAction) {
		connect( closeAction, SIGNAL(cancelled()), closeAction, SLOT(deleteLater()) );
		connect( closeAction, SIGNAL(failed()), closeAction, SLOT(deleteLater()) );
		connect( closeAction, SIGNAL(succeeded()), closeAction, SLOT(deleteLater()) );

		closeAction->start();

	} else {
		AMErrorMon::alert(this, AMSLIT_CANNOT_CLOSE, "Cannot close the slit, closing for this slit is not supported.");
	}
}

void AMSlit::updateGap()
{
	if (gap_) {

		// Update the gap control's first blade.

		if (firstBlade_ && bladeOrientationMap_.contains(firstBlade_))
			gap_->setFirstBlade(firstBlade_, bladeOrientationMap_.value(firstBlade_));
		else
			gap_->removeFirstBlade();

		// Update the gap control's second blade.

		if (secondBlade_ && bladeOrientationMap_.contains(secondBlade_))
			gap_->setSecondBlade(secondBlade_, bladeOrientationMap_.value(secondBlade_));
		else
			gap_->removeSecondBlade();
	}
}

void AMSlit::updateCenter()
{
	if (center_) {

		// Update the center control's first blade.

		if (firstBlade_ && bladeOrientationMap_.contains(firstBlade_))
			center_->setFirstBlade(firstBlade_, bladeOrientationMap_.value(firstBlade_));
		else
			center_->removeFirstBlade();

		// Update the center control's second blade.

		if (secondBlade_ && bladeOrientationMap_.contains(secondBlade_))
			center_->setSecondBlade(secondBlade_, bladeOrientationMap_.value(secondBlade_));
		else
			center_->removeSecondBlade();
	}
}

bool AMSlit::firstBladeCanOpen() const
{
	return (firstBlade_ && firstBlade_->canMove() && bladeOpenValueMap_.contains(firstBlade_));
}

bool AMSlit::secondBladeCanOpen() const
{
	return (secondBlade_ && secondBlade_->canMove() && bladeOpenValueMap_.contains(secondBlade_));
}

bool AMSlit::firstBladeCanClose() const
{
	return (firstBlade_ && firstBlade_->canMove() && bladeClosedValueMap_.contains(firstBlade_));
}

bool AMSlit::secondBladeCanClose() const
{
	return (secondBlade_ && secondBlade_->canMove() && bladeClosedValueMap_.contains(secondBlade_));
}
