#ifndef VESPERSSCANCONFIGURATIONVIEWHOLDER3_H
#define VESPERSSCANCONFIGURATIONVIEWHOLDER3_H

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"

/// This class is a simple extension to the normal configuration view holder specifically for XAS scans.
class VESPERSScanConfigurationViewHolder3 : public AMScanConfigurationViewHolder3
{
	Q_OBJECT

public:
	/// Constructor.  Requires the VESPERSEXAFSScanConfigurationView since it is specifically that scan that needs the ability to be put inside a loop action.
	VESPERSScanConfigurationViewHolder3(VESPERSEXAFSScanConfigurationView *view = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSScanConfigurationViewHolder3();

protected:
	/// This function is used to create the action that will be added to the queue.  The difference between this method and the normal one is the creation of a loop and putting the scan inside.
	virtual AMAction3 *createAction();
};

#endif // VESPERSSCANCONFIGURATIONVIEWHOLDER3_H
