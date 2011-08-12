#include "VESPERSIntermediateSlits.h"

VESPERSIntermediateSlits::VESPERSIntermediateSlits(QObject *parent) :
	QObject(parent)
{
	x_ = new AMProcessVariable("07B2_Int_X:Gap", true, this);
	z_ = new AMProcessVariable("07B2_Int_Z:Gap", true, this);

	connect(x_, SIGNAL(valueChanged(double)), this, SIGNAL(gapXChanged(double)));
	connect(z_, SIGNAL(valueChanged(double)), this, SIGNAL(gapZChanged(double)));
}
