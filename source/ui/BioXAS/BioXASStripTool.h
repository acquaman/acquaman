#ifndef BIOXASSTRIPTOOL_H
#define BIOXASSTRIPTOOL_H

#include "ui/AMStripToolWidget.h"
#include "beamline/AMPVControl.h"

class BioXASStripToolWidget : public AMStripToolWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASStripToolWidget(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASStripToolWidget();

	/// Returns the size hint.
	virtual QSize sizeHint() const;
	/// Returns the desired height for the given width. Reimplemented to make the striptool square.
	virtual int heightForWidth(int width) const;

protected:
	/// Storage ring current control.
	AMReadOnlyPVControl *ringCurrent_;
};

#endif // BIOXASSTRIPTOOL_H
