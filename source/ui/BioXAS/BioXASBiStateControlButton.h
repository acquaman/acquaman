#ifndef BIOXASBISTATECONTROLBUTTON_H
#define BIOXASBISTATECONTROLBUTTON_H

#include "ui/beamline/AMControlToolButton.h"

class BioXASBiStateGroup;

class BioXASBiStateControlButton : public AMControlToolButton
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASBiStateControlButton(BioXASBiStateGroup *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBiStateControlButton();

protected:
	/// Returns the (desired) current background color.
	virtual QColor currentColor() const;
};

#endif // BIOXASBISTATECONTROLBUTTON_H
