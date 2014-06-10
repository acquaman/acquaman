#ifndef IDEASSCANCONFIGURATIONVIEWHOLDER3_H
#define IDEASSCANCONFIGURATIONVIEWHOLDER3_H

#include <QObject>

#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"

/// This class is a simple extension to the normal configuration view holder specifically for XAS scans.
class IDEASScanConfigurationViewHolder3 : public AMScanConfigurationViewHolder3
{
	Q_OBJECT

public:
	/// Constructor.  Requires the VESPERSEXAFSScanConfigurationView since it is specifically that scan that needs the ability to be put inside a loop action.
	virtual ~IDEASScanConfigurationViewHolder3();
	IDEASScanConfigurationViewHolder3(IDEASXASScanConfigurationView *view = 0, QWidget *parent = 0);

protected:
	/// This function is used to create the action that will be added to the queue.  The difference between this method and the normal one is the creation of a loop and putting the scan inside.
	virtual AMAction3 *createAction();
};

#endif // IDEASSCANCONFIGURATIONVIEWHOLDER3_H
