#include "BioXASShuttersButton.h"
#include "beamline/BioXAS/BioXASShutters.h"

BioXASShuttersButton::BioXASShuttersButton(AMControl *shutters, QWidget *parent) :
	AMControlToolButton(shutters, parent)
{
	// Initialize inherited variables.

	setIcon(QIcon(":/shutterIcon2.png"));
	setToolTip("Shutters");
}

BioXASShuttersButton::~BioXASShuttersButton()
{

}

QColor BioXASShuttersButton::currentColor() const
{
	QColor color = QColor(Qt::yellow);

	BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control_);

	if (shutters && shutters->isConnected()) {
		if (shutters->isOpen())
			color = QColor(Qt::green);
		else
			color = QColor(Qt::red);
	}

	return color;
}
