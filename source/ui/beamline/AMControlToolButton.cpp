#include "AMControlToolButton.h"

AMControlToolButton::AMControlToolButton(AMControl *control, QWidget *parent) :
    AMToolButton(parent)
{
	control_ = 0;

	qRegisterMetaType<AMToolButton::ColorState>("AMToolButtonColorState");
	qRegisterMetaType<AMToolButtonColorStateList>("AMToolButtonColorStateList");

	// Current settings.

	setControl(control);
}

AMControlToolButton::~AMControlToolButton()
{

}

void AMControlToolButton::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_)
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateColorState()) );

		emit controlChanged(control_);
	}

	updateColorState();
}

void AMControlToolButton::addColorState(AMToolButton::ColorState state, double minValue, double maxValue)
{
	colorStates_.append(state);
	colorStateMinValues_.append(minValue);
	colorStateMaxValues_.append(maxValue);

	emit colorStatesChanged();
	emit colorStateMinValuesChanged();
	emit colorStateMaxValuesChanged();

	updateColorState();
}

void AMControlToolButton::clearColorStates()
{
	colorStates_.clear();
	colorStateMinValues_.clear();
	colorStateMaxValues_.clear();

	emit colorStatesChanged();
	emit colorStateMinValuesChanged();
	emit colorStateMaxValuesChanged();

	updateColorState();
}

void AMControlToolButton::setColorStatesList(const AMToolButtonColorStateList &newStates)
{
	colorStates_ = newStates;
	emit colorStatesChanged();

	updateColorState();
}

void AMControlToolButton::setColorStateMinValuesList(const AMDoubleList &newValues)
{
	colorStateMinValues_ = newValues;
	emit colorStateMinValuesChanged();

	updateColorState();
}

void AMControlToolButton::setColorStateMaxValuesList(const AMDoubleList &newValues)
{
	colorStateMaxValues_ = newValues;
	emit colorStateMaxValuesChanged();

	updateColorState();
}

void AMControlToolButton::updateColorState()
{
	setColorState( getColorState() );
}
#include <QDebug>
void AMControlToolButton::mouseReleaseEvent(QMouseEvent *e)
{
	qDebug() << "\n\n";

	updateColorState();

	qDebug() << "\n\nColor state:" << colorStateToString(colorState());
	qDebug() << "Color states:" << colorStates_;
	qDebug() << "Color state mins:" << colorStateMinValues_;
	qDebug() << "Color state maxs:" << colorStateMaxValues_;

	AMToolButton::mouseReleaseEvent(e);
}

AMToolButton::ColorState AMControlToolButton::getColorState() const
{
	AMToolButton::ColorState result = None;

	qDebug() << (control_ ? QString("%1: %2").arg(control_->name()).arg(control_->value()) : "AMControlToolButton: no control");

	if (control_ && control_->canMeasure()) {
		double controlValue = control_->value();

		QList<AMToolButton::ColorState> colorStateMatches;

		if (colorStates_.count() == colorStateMinValues_.count() && colorStates_.count() == colorStateMaxValues_.count()) {

			// Iterate through the list of color states, identifying
			// states where the control value falls between the state min
			// and max values.

			for (int i = 0, count = colorStates_.count(); i < count; i++) {
				if (colorStateMinValues_.at(i) <= controlValue && controlValue <= colorStateMaxValues_.at(i))
					colorStateMatches << colorStates_.at(i);
			}
		}

		// If there is more than one potential color state match,
		// take the result to be the first one.

		if (colorStateMatches.count() >= 1) {
			qDebug() << "Color state match found:" << colorStateToString(colorStateMatches.at(0));
			result = colorStateMatches.first();
		} else {
			qDebug() << "No color state matches found :(";
		}
	}

	return result;
}
