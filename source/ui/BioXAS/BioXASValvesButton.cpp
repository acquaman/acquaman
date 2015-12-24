#include "BioXASValvesButton.h"
#include "beamline/BioXAS/BioXASValves.h"

BioXASValvesButton::BioXASValvesButton(AMControl *valves, QWidget *parent) :
	AMControlToolButton(valves, parent)
{
	// Initialize inherited variables.

	setIcon(QIcon(":/valveIcon2.png"));
	setToolTip("Valves");
}

BioXASValvesButton::~BioXASValvesButton()
{

}

QColor BioXASValvesButton::currentColor() const
{
	QColor color = QColor(Qt::yellow);

	BioXASValves *valves = qobject_cast<BioXASValves*>(control_);

	if (valves && valves->isConnected()) {
		if (valves->isOpen())
			color = QColor(Qt::green);
		else
			color = QColor(Qt::red);
	}

	return color;
}
