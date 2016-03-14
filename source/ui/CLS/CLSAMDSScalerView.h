#ifndef CLSAMDSSCALERVIEW_H
#define CLSAMDSSCALERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QHBoxLayout>

class CLSAMDSScaler;
class CLSAMDSScalerChannel;

#include "source/ui/beamline/AMCurrentAmplifierSingleView.h"

/*!
  This class builds a view around a CLSAMDSScalerChannel.  It provides a simple view that has a check box
  and the current reading.
  */

class CLSAMDSScalerChannelView : public QWidget
{
Q_OBJECT

public:
	/// Simple enum that enumerates the two modes that the output can be viewed.
	enum OutputViewMode { Counts = 0, Voltage = 1 };

	/// Constructor.  Takes a CLSSIS3820ScalarChannel and builds a view around it.
	CLSAMDSScalerChannelView(CLSAMDSScalerChannel *channel, QWidget *parent = 0);
	virtual ~CLSAMDSScalerChannelView();

	/// Whether or not the view has a connected AmplifierView
	bool hasAmplifierView() const;

signals:
	/// Notifier that the channel changed viewing mode.  Passes the new mode.
	void outputViewModeChanged(CLSAMDSScalerChannelView::OutputViewMode);
	/// Notifier that passes along that the amplifier view has changed.
	void amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode);

public slots:
	/// Convenience method that sets the output view mode for the channel.
	virtual void setOutputViewMode(CLSAMDSScalerChannelView::OutputViewMode mode);
	/// Convenience method that sets the view mode of the current amplifier.
	void setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode);
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
	/// Sets the precision for the composite view.
	void setAmplifierViewPrecision(int newPrecision);
	/// Sets the format for the composite view.
	void setAmplifierViewFormat(char newFormat);

protected slots:
	/// Handles building the appropriate string when the reading changed.
	virtual void onReadingChanged();
	/// Handles changing the view mode when the scaler output button is clicked.
	void onScalerOutputClicked();
	/// Handles updating the status label.  Includes the color and updating the tool tip.
	void updateStatusLabel();
	/// Handles setting up the SR570 view if it is added after the channel has been set inside the view.
	void onNewCurrentAmplifierAttached();

protected:
	/// Pointer to the channel being viewed.
	CLSAMDSScalerChannel *channel_;

	/// Label holding the current reading of the scaler channel.  Depending on it's state, will display either counts or voltage.
	QToolButton *scalerOutput_;
	/// The check box showing whether the channel is enabled or not.
	QCheckBox *enableBox_;
	/// Flag holding what the output should be.
	OutputViewMode viewMode_;
	/// Label holding the name of the scaler channel.
	QLabel *channelName_;
	/// Holds the SR570, should there be a SR570 bound to this channel.
	AMCurrentAmplifierSingleView *amplifierView_;
	/// Holds the label for the status.
	QLabel *statusLabel_;
	/// The layout that holds all elements of the view.
	QHBoxLayout *channelLayout_;
};

class CLSAMDSScalerControlsView;

/*!
	This class builds a view for the SIS3820 scalar used throughout the CLS.  It takes a scalar object and builds a standard view
	around it that currently is modeled after the current Scaler app view.
  */
class CLSAMDSScalerView : public QWidget
{

Q_OBJECT

public:
	/// Constructor.  Takes a scalar object.
	CLSAMDSScalerView(CLSAMDSScaler *scaler, QWidget *parent = 0);
	virtual ~CLSAMDSScalerView();

	/// Sets the precision for the composite view.
	void setAmplifierViewPrecision(int newPrecision);
	/// Sets the format for the composite view.
	void setAmplifierViewFormat(char newFormat);

protected slots:
	/// Handles switching all the SR570 views to match the most recent change.
	void onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode mode);
	/// Handles switching all the output views to match the most recent change.
	void onOutputViewModeChanged(CLSAMDSScalerChannelView::OutputViewMode mode);

protected:
	/// Pointer to the scalar being viewed.
	CLSAMDSScaler *scaler_;

	/// The scaler controls view.
	CLSAMDSScalerControlsView *controlsView_;
	/// Holds all of the individual channel views.
	QList<CLSAMDSScalerChannelView *> channelViews_;

	/// A layout for all of the channel views
	QVBoxLayout *channelLayout_;
	/// The main layout for the widget
	QVBoxLayout *mainVL_;
};

class CLSAMDSScalerControlsView : public QWidget
{
Q_OBJECT

public:
	/// Constructor.
	explicit CLSAMDSScalerControlsView(CLSAMDSScaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSAMDSScalerControlsView();
	/// Returns the scaler being viewed.
	CLSAMDSScaler* scaler() const { return scaler_; }

signals:

public slots:
	/// Sets the scaler being viewed.
	void setScaler(CLSAMDSScaler *newScaler);

protected slots:
	/// Starts a scan.
	void startScanning();
	/// Stops a scan.
	void stopScanning();
	/// Sets the scaler scan mode, using the view input.
	void setDwellMode();
	/// Sets the scaler dwell time, using the view input.
	void setDwellTime();

	/// Handles updating the mode choice, in response to the scaler's continous mode status changed.
	void onDwellModeChanged();
	/// Handles updating the dwell time, in response to the scaler's change in time.
	void onDwellTimeChanged();
	/// Handles updating the dwell state, in resposne to the scaler's change
	void onDwellStateChanged();

	void onScalerConnected(bool connected);

protected:
	/// The scaler being viewed.
	CLSAMDSScaler *scaler_;

	/// Button that handles starting the scanning mode of the scaler.
	QPushButton *startButton_;
	/// Button that handles stopping the scanning mode of the scaler.
	QPushButton *stopButton_;
	/// Button that handles setting the mode of the scaler.
	QComboBox *modeChoice_;
	/// Label holding the overal scanning status of the scaler.  Matches the scanning button.
	QLabel *status_;
	/// Spin box holding the amount of time to per point.
	QDoubleSpinBox *time_;
};

#endif // CLSAMDSSCALERVIEW_H
