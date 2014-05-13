#ifndef SGMSIS3820SCALERVIEW_H
#define SGMSIS3820SCALERVIEW_H

#include "ui/CLS/CLSSIS3820ScalerView.h"

class SGMSIS3820ScalerView : public CLSSIS3820ScalerView
{
Q_OBJECT

public:
	/// Constructor.  Takes a scalar object.
 	virtual ~SGMSIS3820ScalerView();
	SGMSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
};

#endif // SGMSIS3820SCALERVIEW_H
