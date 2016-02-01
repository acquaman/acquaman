#ifndef BIOXASSIS3820SCALERCHANNELVIEW_H
#define BIOXASSIS3820SCALERCHANNELVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

class AMExtendedControlEditor;

class BioXASSIS3820ScalerChannelView : public CLSSIS3820ScalerChannelView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, bool biasEnabledVisible = false, bool biasVisible = false, bool darkCurrentVisible = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerChannelView();
	/// Returns the channel being viewed.
	CLSSIS3820ScalerChannel* channel() const { return channel_; }
	/// Returns whether the bias enabled editor is shown.
	bool biasEnabledVisible() const { return biasEnabledVisible_; }
	/// Returns whether the bias editor is shown.
	bool biasVisible() const { return biasVisible_; }
	/// Returns whether the dark current is shown.
	bool darkCurrentVisible() const { return darkCurrentVisible_; }

public slots:
	/// Sets whether the bias enabled editor is shown.
	void setBiasEnabledEditorVisible(bool isVisible);
	/// Sets whether the bias editor is shown.
	void setBiasEditorVisible(bool isVisible);
	/// Sets whether the dark current is shown.
	void setDarkCurrentVisible(bool isVisible);
	/// Sets the value of the dark current display.
	void setDarkCurrentValue(double newValue);
	/// Sets whether the dark current displayed is valid (black) or invalid (red).
	void setDarkCurrentState(bool valid);

protected:
	/// Bool indicating whether the bias enabled editor should be shown.
	bool biasEnabledVisible_;
	/// Bool indicating whether the bias editor should be shown.
	bool biasVisible_;
	/// Bool indicating whether the dark current value should be shown.
	bool darkCurrentVisible_;

	/// The bias enabled editor.
	AMExtendedControlEditor *biasEnabledEditor_;
	/// The bias editor.
	AMExtendedControlEditor *biasEditor_;
	/// The dark current value display.
	QLabel *darkCurrentDisplay_;
};

#endif // BIOXASSIS3820SCALERCHANNELVIEW_H
