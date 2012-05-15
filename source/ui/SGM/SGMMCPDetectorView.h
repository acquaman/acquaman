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


#ifndef SGMMCPDETECTORVIEW_H
#define SGMMCPDETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"

class SGMMCPBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit SGMMCPBriefDetectorView(SGMMCPDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	AMDetector* detector();

protected slots:
	void onPoweredOnChanged(bool poweredOn);

protected:
	AMControlEditor *readingCE_;
	QToolButton *powerState_;
	QIcon powerOnState_, powerOffState_;
	QHBoxLayout *hl_;
	SGMMCPDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class SGMMCPDetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit SGMMCPDetailedDetectorView(SGMMCPDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/* NTBA March 14, 2011 David Chevrier
	   Needs a destructor for configurationSettings_
	   */

	AMDetector* detector();

	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;

protected slots:
	void onControlSetpointRequested();

protected:
	AMControlEditor *readingCE_;
	AMControlEditor *hvCE_;
	QGridLayout *gl_;
	SGMMCPDetector *detector_;
	SGMMCPDetectorInfo *configurationSettings_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // SGMMCPDETECTORVIEW_H
