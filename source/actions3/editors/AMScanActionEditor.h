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


#ifndef AMSCANACTIONEDITOR_H
#define AMSCANACTIONEDITOR_H

#include <QFrame>
#include <actions3/actions/AMScanActionInfo.h>

class QLabel;

/// This class implements a simple editor widget for the AMScanActionInfo that can be shown inside action tree views.  It provides the ability to open up the configuration for the scan.
class AMScanActionEditor : public QFrame
{
	Q_OBJECT

public:
	/// Constructor: Must be Q_INVOKABLE, and accept an AMScanActionInfo as argument
	Q_INVOKABLE AMScanActionEditor(AMScanActionInfo *info, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanActionEditor();

signals:

public slots:

protected slots:
	/// Helper slot that brings up the configuration for the scan.
	void onScanConfigurationViewRequested();
	/// Helper slot that changes the description of the scan if the info changes
	void onScanInfoChanged();
protected:
	/// Label that holds the name of the scan.
	QLabel *scanName_;
	/// Pointer to the info for this scan action.
	AMScanActionInfo *info_;
	/// Pointer to the configuration view.  This is to ensure that only one view is generated and you can't accidentally have 10 identical scan view's open simultaneously.
	QWidget *configView_;
};

#endif // AMSCANACTIONEDITOR_H
