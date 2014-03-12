#ifndef CLSSIS3820COMPOSITESCALERCHANNELVIEW_H
#define CLSSIS3820COMPOSITESCALERCHANNELVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QHBoxLayout>

class CLSSIS3820ScalerChannel;

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/CLS/CLSSR570CompositeView.h"

/// This view mimics the standard CLSSIS3820ScalerChannelView, but uses two scaler channels instead of one.
class CLSSIS3820CompositeScalerChannelView : public QWidget
{
	Q_OBJECT

public:
	/// Simple enum that enumerates the two modes that the output can be viewed.
	enum OutputViewMode { Counts = 0, Voltage = 1 };

	/// Constructor.  Takes two CLSSIS3820ScalerChannels and builds a view around them.
 	virtual ~CLSSIS3820CompositeScalerChannelView();
	explicit CLSSIS3820CompositeScalerChannelView(CLSSIS3820ScalerChannel *channel1, CLSSIS3820ScalerChannel *channel2, QWidget *parent = 0);

signals:
	/// Notifier that the channel changed viewing mode.  Passes the new mode.
	void outputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode);
	/// Notifier that passes along that the SR570 view changed.
	void sr570ViewModeChanged(CLSSR570CompositeView::ViewMode);

public slots:
	/// Sets the custom name for the composite scaler channel view.
	void setCustomCompositeName(const QString &name);
	/// Convenience method that sets the output view mode for the channel.
	void setOutputViewMode(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode);
	/// Convenience method that sets the view mode of the SR570.
	void setSR570ViewMode(CLSSR570CompositeView::ViewMode mode);
	/// Sets the visibility for the enable check box.
	void setEnableCheckBoxVisibility(bool visible);
	/// Sets the custom name visibility.
	void setCustomNameVisibility(bool visible);
	/// Sets the SR570 visibility (does nothing if no SR570 is set).
	void setSR570Visibility(bool visible);
	/// Sets the output visibility.
	void setOutputVisibility(bool visible);
	/// Sets the status label visibility.
	void setStatusLabelVisibility(bool visible);

protected slots:
	/// Handles building the appropriate string when the reading changed.
	void onReadingChanged();
	/// Handles changing the view mode when the scaler output button is clicked.
	void onScalerOutputClicked();
	/// Handles updating the status label.  Includes the color and updating the tool tip.
	void updateStatusLabel();
	/// Handles setting up the SR570 view if it is added after the channel has been set inside the view.
	void onNewSR570Attached();
	/// Handles updating the name label when switching between views modes.
	void updateCompositeNameLabel();
	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);

protected:
	/// Pointer to the first channel being viewed.
	CLSSIS3820ScalerChannel *channel1_;
	/// Pointer to the second channel being viewed.
	CLSSIS3820ScalerChannel *channel2_;

	/// Flag holding whether the view should show the individual elements or the composite view.
	bool showCompositeView_;
	/// The custom name to be given to the composite scaler channel.
	QString compositeName_;

	/// Label holding the current reading of the scaler channel.  Depending on it's state, will display either counts or voltage.
	QToolButton *scalerOutput_;
	/// The check box showing whether the channel is enabled or not.
	QCheckBox *enableBox_;
	/// Flag holding what the output should be.
	OutputViewMode viewMode_;
	/// Label holding the name of the scaler channel.
	QLabel *channelName_;
	/// Holds the SR570, should there be a SR570 bound to this channel.
	CLSSR570CompositeView *sr570View_;
	/// Holds the label for the status.
	QLabel *statusLabel_;
	/// The layout that holds all elements of the view.
	QHBoxLayout *channelLayout_;
};

#endif // CLSSIS3820COMPOSITESCALERCHANNELVIEW_H