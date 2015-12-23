#include "BioXASValvesButton.h"
#include "beamline/BioXAS/BioXASValves.h"

BioXASValvesButton::BioXASValvesButton(BioXASValves *valves, QWidget *parent) :
    QToolButton(parent)
{
	// Initialize inherited variables.

	setCheckable(true);
	setAutoRaise(false);
	setIcon(QIcon(":/valveIcon2.png"));
	setToolTip("Valves");

	// Initialize class variables.

	valves_ = 0;

	// Current settings.

	setValves(valves);
	refresh();
}

BioXASValvesButton::~BioXASValvesButton()
{

}

void BioXASValvesButton::refresh()
{
	// Identify color.

	QColor buttonColor = QColor(Qt::yellow);

	if (valves_ && valves_->isConnected()) {
		if (valves_->isOpen())
			buttonColor = QColor(Qt::green);
		else
			buttonColor = QColor(Qt::red);
	}

	// Update button color.

	setPalette(QPalette(buttonColor));
}

void BioXASValvesButton::setValves(BioXASValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			disconnect( valves_, 0, this, 0 );

		valves_ = newValves;

		if (valves_) {
			connect( valves_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( valves_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit valvesChanged(valves_);
	}
}
