#ifndef BIOXASSTRIPTOOL_H
#define BIOXASSTRIPTOOL_H

#include "beamline/AMPVControl.h"
#include "ui/AMStripTool.h"

class BioXASStripTool : public AMStripTool
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASStripTool(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASStripTool();

	/// Returns the size hint.
	virtual QSize sizeHint() const;
	/// Returns the desired height for the given width. Reimplemented to make the striptool square.
	virtual int heightForWidth(int width) const;

protected:
	/// Storage ring current control.
	AMReadOnlyPVControl *ringCurrent_;
};

#endif // BIOXASSTRIPTOOL_H
