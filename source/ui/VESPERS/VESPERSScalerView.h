#ifndef VESPERSSCALERVIEW_H
#define VESPERSSCALERVIEW_H

#include <QWidget>

#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSSIS3820CompositeScalerChannelView.h"

/// Simple scaler view.  Builds a composite scaler channel view for the split ion chamber and uses the typical scaler channel for the others.
class VESPERSScalerView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the view for the 5 scaler channels that users care about.
 	virtual ~VESPERSScalerView();
	explicit VESPERSScalerView(QWidget *parent = 0);

signals:

protected slots:
	/// Handles switching all the SR570 views to match the most recent change.
	void onSR570ViewChanged(CLSSR570View::ViewMode mode);
	/// Handles switching all the SR570 views to match the most recent change.
	void onSR570ViewChanged(CLSSR570CompositeView::ViewMode mode);
	/// Handles switching all the output views to match the new composite view mode.
	void onOutputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode);
	/// Handles switching all the output views to match the most recent change.
	void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode);

protected:
	/// Holds the list of scaler channel views.
	QList<CLSSIS3820ScalerChannelView *> singleViews_;
	/// Holds the pointer to the composite channel scaler view.
	CLSSIS3820CompositeScalerChannelView *compositeView_;
};

#endif // VESPERSSCALERVIEW_H
