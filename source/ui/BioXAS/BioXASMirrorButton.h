#ifndef BIOXASMIRRORBUTTON_H
#define BIOXASMIRRORBUTTON_H

#include "ui/beamline/AMControlToolButton.h"

class BioXASMirrorButton : public AMControlToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorButton();

protected:
	/// Returns the (desired) current background color.
	virtual QColor currentColor() const;

};

#endif // BIOXASMIRRORBUTTON_H
