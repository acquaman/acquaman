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


#ifndef AMDATAMANAPPBOTTOMPANEL_H
#define AMDATAMANAPPBOTTOMPANEL_H

#include "ui/AMBottomPanel.h"
#include "ui/AMStatusView.h"

#include <QToolButton>

/// This class implements the bottom panel used for the dataman application.  It adds the ability to add experiments and show the status view.
class AMDatamanAppBottomPanel : public AMBottomPanel
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMDatamanAppBottomPanel();
	AMDatamanAppBottomPanel(QWidget *parent = 0);

signals:
	/// Signal used to tell the dataview that it should add an experiment.
	void addExperimentButtonClicked();

protected:
	/// The status view.
	AMStatusView *statusView_;
};

#endif // AMDATAMANAPPBOTTOMPANEL_H
