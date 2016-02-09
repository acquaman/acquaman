#ifndef BIOXASMONOCHROMATORBUTTON_H
#define BIOXASMONOCHROMATORBUTTON_H

#include "ui/beamline/AMControlToolButton.h"

class BioXASMonochromatorButton : public AMControlToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMonochromatorButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromatorButton();

protected:
	/// Returns the (desired) current background color.
	virtual QColor currentColor() const;
};

#endif // BIOXASMONOCHROMATORBUTTON_H
