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


#ifndef AMSINGLECONTROLDETECTORVIEW_H
#define AMSINGLECONTROLDETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"
#include "beamline/AMSingleControlDetector.h"

class AMSingleControlBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMSingleControlBriefDetectorView(AMSingleControlDetector *detector = 0, QWidget *parent = 0);

	/* NTBA March 14, 2011 David Chevrier
	   Needs a Destructor
	   */

	AMDetector* detector();

	AMDetectorInfo* configurationSettings() const;

protected:
	AMExtendedControlEditor *fbk_;
	QHBoxLayout *hl_;
	AMSingleControlDetector *detector_;
	AMDetectorInfo *configurationSettings_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class AMSingleReadOnlyControlBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMSingleReadOnlyControlBriefDetectorView(AMSingleReadOnlyControlDetector *detector = 0, QWidget *parent = 0);

	/* NTBA March 14, 2011 David Chevrier
	   Needs a Destructor
	   */

	AMDetector* detector();

	AMDetectorInfo* configurationSettings() const;

protected:
	AMExtendedControlEditor *fbk_;
	QHBoxLayout *hl_;
	AMSingleReadOnlyControlDetector *detector_;
	AMDetectorInfo *configurationSettings_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // AMSINGLECONTROLDETECTORVIEW_H
