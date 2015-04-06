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
	explicit BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, bool biasEnabledShown = false, bool biasShown = false, bool darkCurrentShown = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerChannelView();
	/// Returns the channel being viewed.
	CLSSIS3820ScalerChannel* channel() const { return channel_; }
	/// Returns whether the bias enabled editor is shown.
	bool biasEnabledShown() const { return biasEnabledShown_; }
	/// Returns whether the bias editor is shown.
	bool biasShown() const { return biasShown_; }
	/// Returns whether the dark current is shown.
	bool darkCurrentShown() const { return darkCurrentShown_; }

public slots:
	/// Sets whether the bias enabled editor is shown.
	void setBiasEnabledEditorShown(bool shown);
	/// Sets whether the bias editor is shown.
	void setBiasEditorShown(bool shown);
	/// Sets whether the dark current is shown.
	void setDarkCurrentShown(bool shown);
	/// Sets the value of the dark current display.
	void setDarkCurrentValue(double newValue);
	/// Sets whether the dark current displayed is valid (black) or invalid (red).
	void setDarkCurrentState(bool valid);

protected:
	/// Bool indicating whether the bias enabled editor should be shown.
	bool biasEnabledShown_;
	/// Bool indicating whether the bias editor should be shown.
	bool biasShown_;
	/// Bool indicating whether the dark current value should be shown.
	bool darkCurrentShown_;

	/// The bias enabled editor.
	AMExtendedControlEditor *biasEnabledEditor_;
	/// The bias editor.
	AMExtendedControlEditor *biasEditor_;
	/// The dark current value display.
	QLabel *darkCurrentDisplay_;
};

#endif // BIOXASSIS3820SCALERCHANNELVIEW_H
