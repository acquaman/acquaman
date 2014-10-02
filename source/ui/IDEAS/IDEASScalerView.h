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


#ifndef IDEASSCALERVIEW_H
#define IDEASSCALERVIEW_H

#include <QWidget>

#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSSIS3820CompositeScalerChannelView.h"

/// Simple scaler view.  Builds a composite scaler channel view for the split ion chamber and uses the typical scaler channel for the others.
class IDEASScalerView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the view for the 5 scaler channels that users care about.
	virtual ~IDEASScalerView();
	explicit IDEASScalerView(QWidget *parent = 0);

signals:

protected slots:
	/// Handles switching all the SR570 views to match the most recent change.
	void onSR570ViewChanged(AMCurrentAmplifierView::ViewMode mode);
	/// Handles switching all the output views to match the new composite view mode.
	void onOutputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode);
	/// Handles switching all the output views to match the most recent change.
	void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode);

protected:
	/// Holds the list of scaler channel views.
	QList<CLSSIS3820ScalerChannelView *> singleViews_;
};

#endif // IDEASSCALERVIEW_H
