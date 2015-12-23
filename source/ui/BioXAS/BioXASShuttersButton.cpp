#include "BioXASShuttersButton.h"
#include "beamline/BioXAS/BioXASShutters.h"

BioXASShuttersButton::BioXASShuttersButton(BioXASShutters *shutters, QWidget *parent) :
    QToolButton(parent)
{
	// Initialize inherited variables.

	setAutoRaise(false);
	setIcon(QIcon(":/shutterIcon2.png"));

	// Initialize class variables.

	shutters_ = 0;

	// Current settings.

	setShutters(shutters);
	refresh();
}

BioXASShuttersButton::~BioXASShuttersButton()
{

}

void BioXASShuttersButton::refresh()
{
	// Identify color.

	QColor buttonColor = QColor(Qt::yellow);

	if (shutters_) {
		if (shutters_->isOpen())
			buttonColor = QColor(Qt::green);
		else
			buttonColor = QColor(Qt::red);
	}

	// Update button color.

//	setStyleSheet(QString("QToolButton:!hover {background-color: %1; border: 1px; border-color: black; border-style: outset; border-radius: 4px;}"
//						  "QToolButton:hover {background-color: %1; border: 1px; border-color: black; border-style: outset; border-radius: 4px}").arg(buttonColor.name()));

//	QPalette widgetPalette = palette();
//	widgetPalette.setColor(backgroundRole(), Qt::red);
//	setPalette(widgetPalette);

	setPalette(QPalette(buttonColor));
}

void BioXASShuttersButton::setShutters(BioXASShutters *newShutters)
{
	if (shutters_ != newShutters) {

		if (shutters_)
			disconnect( shutters_, 0, this, 0 );

		shutters_ = newShutters;

		if (shutters_) {
			connect( shutters_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( shutters_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
		}

		refresh();

		emit shuttersChanged(shutters_);
	}
}
