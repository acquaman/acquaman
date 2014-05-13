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


#ifndef AMOLDDETECTORVIEW_H
#define AMOLDDETECTORVIEW_H

#include "AMControlSetView.h"
#include "AMOldDetectorViewSupport.h"
#include "QMessageBox"
#include "QMetaMethod"

class AMOldDetectorView : public QWidget
{
Q_OBJECT

public:
 	virtual ~AMOldDetectorView();
	Q_INVOKABLE explicit AMOldDetectorView(bool configureOnly = false, QWidget *parent = 0);

	virtual AMOldDetector* detector();

	virtual AMOldDetectorInfo* configurationSettings() const;

public slots:

signals:
	void settingsChangeRequested();
	void settingsConfigureRequested();

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMOldDetector *detector, bool configureOnly = false);
	friend AMOldDetectorView* AMOldDetectorViewSupport::createDetectorView(AMOldDetector *detector, bool configureOnly);
	friend AMOldDetectorView* AMOldDetectorViewSupport::createBriefDetectorView(AMOldDetector *detector, bool configureOnly);
	friend AMOldDetectorView* AMOldDetectorViewSupport::createDetailedDetectorView(AMOldDetector *detector, bool configureOnly);

protected:
	bool configureOnly_;
};

class AMBriefOldDetectorView : public AMOldDetectorView
{
Q_OBJECT
public:
 	virtual ~AMBriefOldDetectorView();
	Q_INVOKABLE explicit AMBriefOldDetectorView(bool configureOnly = false, QWidget *parent = 0);

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMOldDetector *detector, bool configureOnly = false);
};

class AMDetailedOldDetectorView : public AMOldDetectorView
{
Q_OBJECT
public:
 	virtual ~AMDetailedOldDetectorView();
	Q_INVOKABLE explicit AMDetailedOldDetectorView(bool configureOnly = false, QWidget *parent = 0);

protected:
	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	virtual bool setDetector(AMOldDetector *detector, bool configureOnly = false);
};

#endif // AMDETECTORVIEW_H
