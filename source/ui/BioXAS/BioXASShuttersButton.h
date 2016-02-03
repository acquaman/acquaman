#ifndef BIOXASSHUTTERSBUTTON_H
#define BIOXASSHUTTERSBUTTON_H

#include "ui/beamline/AMControlToolButton.h"

class BioXASShutters;

class BioXASShuttersButton : public AMControlToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShuttersButton(AMControl *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersButton();

protected:
	/// Returns the (desired) current background color.
	virtual QColor currentColor() const;
};

#endif // BIOXASSHUTTERSBUTTON_H
