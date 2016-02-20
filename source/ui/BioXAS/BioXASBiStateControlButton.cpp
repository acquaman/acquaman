#include "BioXASBiStateControlButton.h"
#include "beamline/BioXAS/BioXASBiStateGroup.h"

BioXASBiStateControlButton::BioXASBiStateControlButton(BioXASBiStateGroup *control, QWidget *parent) :
	AMControlToolButton(control, parent)
{

}

BioXASBiStateControlButton::~BioXASBiStateControlButton()
{

}

QColor BioXASBiStateControlButton::currentColor() const
{
	QColor color = QColor(Qt::yellow);

	BioXASBiStateGroup *control = qobject_cast<BioXASBiStateGroup*>(control_);

	if (control && control->isConnected()) {
		if (control->inState1())
			color = QColor(Qt::green);
		else
			color = QColor(Qt::red);
	}

	return color;
}

