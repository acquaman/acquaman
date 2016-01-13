#ifndef BIOXASVALVESBUTTON_H
#define BIOXASVALVESBUTTON_H

#include "ui/beamline/AMControlToolButton.h"

class BioXASValves;

class BioXASValvesButton : public AMControlToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValvesButton(AMControl *valves, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesButton();

protected:
	/// Returns the (desired) current background color.
	virtual QColor currentColor() const;
};

#endif // BIOXASVALVESBUTTON_H
