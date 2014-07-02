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


#ifndef AMOLDDETECTORINFOSETVIEW_H
#define AMOLDDETECTORINFOSETVIEW_H

#include <QGroupBox>

#include "dataman/info/AMOldDetectorInfoSet.h"

class QScrollArea;

class AMOldDetectorInfoSetViewInternal;

class AMOldDetectorInfoSetView : public QGroupBox
{
Q_OBJECT
public:
 	virtual ~AMOldDetectorInfoSetView();
	AMOldDetectorInfoSetView(const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);

protected:
	AMOldDetectorInfoSetViewInternal *internalView_;
	QScrollArea *scrollArea_;
};

class AMOldDetectorInfoSetViewInternal : public QWidget
{
Q_OBJECT
public:
 	virtual ~AMOldDetectorInfoSetViewInternal();
	AMOldDetectorInfoSetViewInternal(const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);
};

#endif // AMOLDDETECTORINFOSETVIEW_H
