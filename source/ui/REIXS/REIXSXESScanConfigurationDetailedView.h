/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H
#define REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
#include "ui/REIXS/REIXSXESMCPDetectorView.h"

class REIXSXESScanConfigurationDetailedView : public AMScanConfigurationView
{
	Q_OBJECT
public:
 	virtual ~REIXSXESScanConfigurationDetailedView();
	//explicit REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetectorPre2013* detector, QWidget *parent = 0);
	explicit REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetector* detector, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const { return basicConfigurationView_->configuration(); }

signals:

public slots:

protected:
	REIXSXESMCPDetectorView* detectorView_;
	REIXSXESScanConfigurationView* basicConfigurationView_;


};

#endif // REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H
