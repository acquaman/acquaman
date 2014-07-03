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


#ifndef AMAPPBOTTOMPANEL_H
#define AMAPPBOTTOMPANEL_H

#include "ui/AMDatamanAppBottomPanel.h"
#include "ui/actions3/AMActionRunnerBottomBarCurrentView3.h"
#include "actions3/AMActionRunner3.h"

/// This class implements the bottom panel used in the standard App for AM.  It contains a mini-view of the workflow and adds it between the add experiment button and the status view.
class AMAppBottomPanel : public AMDatamanAppBottomPanel
{
	Q_OBJECT

public:
	/// Constructor.  Passes in the action runner to appropriately build the mini-workflow view.
 	virtual ~AMAppBottomPanel();
	AMAppBottomPanel(AMActionRunner3 *actionRunner, QWidget *parent = 0);

	/// Returns the bottom bar view that is encapuslated.
	AMActionRunnerBottomBarCurrentView3 *workFlowView() const { return workflowView_; }

protected:
	/// The current workflow view.
	AMActionRunnerBottomBarCurrentView3 *workflowView_;
};

#endif // AMAPPBOTTOMPANEL_H
