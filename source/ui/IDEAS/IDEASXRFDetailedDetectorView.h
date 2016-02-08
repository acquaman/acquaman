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


#ifndef IDEASXRFDETAILEDDETECTORVIEW_H
#define IDEASXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include <QPushButton>
#include <QComboBox>

class IDEASXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	IDEASXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~IDEASXRFDetailedDetectorView();

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

protected slots:
	/// Slot for doing a very quick dead time check and then resets the detector to its previous time.
	void onDeadTimeCheckButtonClicked();

protected:
	/// Method that builds the Scan Save Button and associated things.
	void buildScanSaveViews();

	/// button to trigger a 0.1s XRF acquitisition to check to too-high count rates.
	QPushButton *deadTimeCheckButton_;
};

#endif // IDEASXRFDETAILEDDETECTORVIEW_H
