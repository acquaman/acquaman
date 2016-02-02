/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMSCANCONFIGURATIONVIEWHOLDER3_H
#define AMSCANCONFIGURATIONVIEWHOLDER3_H

#include "ui/actions3/AMActionRunnerAddActionBar3.h"

class AMScanConfiguration;
class AMScanConfigurationView;
class AMTopFrame;

/// This widget holds a scan configuration widget, and provides user controls to start the configured scan within the workflow.  It can be constructed to surround any AMScanConfigurationView. Below the widget, it adds buttons to start the configured scan or add it to the workflow queue (using AMActionRunner).
/*! This class is an updated version of the old scan configuration view holder that uses the workflow system in actions3/AMActionRunner.

This widget takes ownership of the view specifified in its constructor.

When the "Start Scan" button or "Add to Workflow" button is pressed, a copy of the view's current scan configuration is made, and an AMScanControllerAction (based on this config) is what gets added to the queue.  If you want to specify a different kind of action, you can overload the protected createAction() function.
*/
class AMScanConfigurationViewHolder3 : public AMActionRunnerAddActionBar3
{
	Q_OBJECT
public:
	/// Construct a holder for an AMScanConfigurationView \c view (or 0 if no view is ready yet).
	/*! If provided, this widget takes ownership of the \c view */
 	virtual ~AMScanConfigurationViewHolder3();
	AMScanConfigurationViewHolder3(AMScanConfigurationView* view = 0, bool enableLoopAction = false, QWidget *parent = 0);

	/// Construct a holder for AMScanConfigurationView. The configuration view holder adds a frame, and squeezes view in
	AMScanConfigurationViewHolder3(const QString &frameName, bool enableLoopAction, bool squeezeWidget, AMScanConfigurationView *view = 0,  const QString &iconName = ":/utilities-system-monitor.png", QWidget *parent = 0);

	/// Set the \c view contained within the holder.  If there is an existing view, the old view is deleted.
	/*! You can pass in 0 to remove the existing view from the holder.*/
	void setView(AMScanConfigurationView* view, const QString &frameName = "", bool enableLoopAction = false, bool squeezeWidget = false, const QString &iconName = ":/utilities-system-monitor.png");

	/// Set the \c view as enabled or disabled. If there is a scan configuration view present, a red overlay warning will be present when set as disabled
	void setEnabled(bool enabled);

protected:

	/// This function is used to create the action that will be added to the queue. By default, it creates an AMScanControllerAction using a copy of the view's scan configuration. You can re-implement this to provide a specific kind of action (for ex: REIXSXESScanControllerAction, etc.) for use with the action registry system.
	virtual AMAction3* createScan();
	/// This function is used to create the loop action that will be added to the queue.
	virtual AMAction3* createMultipleScans();

	/// This is the scan configuration widget we're wrapping
	AMScanConfigurationView* view_;

	/// Warning label for disabled state
	QLabel *disabledWarning_;

	AMTopFrame *topFrame_;
};

#endif // AMSCANCONFIGURATIONVIEWHOLDER3_H
