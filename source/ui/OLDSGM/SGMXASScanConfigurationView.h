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


#ifndef AM_SGMXASScanConfigurationViewer_H
#define AM_SGMXASScanConfigurationViewer_H

#include "AMQtGui.h"

class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QSpacerItem;

#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/AMTopFrame.h"

class SGMFluxResolutionPickerStaticView;
class AMControlInfoListView;
class AMOldDetectorInfoSetView;
class AMStepScanAxisView;

class SGMXASScanConfigurationView : public AMScanConfigurationView{
Q_OBJECT
public:
	SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent = 0);
	virtual ~SGMXASScanConfigurationView();

	const AMScanConfiguration* configuration() const;

public slots:
	void setDisabled(bool disabled);

protected:
	SGMXASScanConfiguration *cfg_;

	AMTopFrame *topFrame_;
	AMStepScanAxisView *regionsView_;
	SGMFluxResolutionPickerStaticView *fluxResolutionView_;
	AMControlInfoListView *trackingStaticView_;
	AMOldDetectorInfoSetView *xasDetectorsStaticView_;

	QVBoxLayout *mainVL_;
	QGridLayout *bottomGL_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
