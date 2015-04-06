#ifndef BIOXASSIS3820SCALERCHANNELSVIEW_H
#define BIOXASSIS3820SCALERCHANNELSVIEW_H

#include <QWidget>

#include "ui/BioXAS/BioXASSIS3820ScalerChannelView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class BioXASSIS3820ScalerChannelsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerChannelsView(CLSSIS3820Scaler *scaler, bool biasEnabledShown = false, bool biasShown = false, bool darkCurrentShown = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerChannelsView();
	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns whether the bias enabled editor is shown, for all able channels.
	bool biasEnabledEditorShown() const { return biasEnabledEditorShown_; }
	/// Returns whether the bias editor is shown, for all able channels.
	bool biasEditorShown() const { return biasEditorShown_; }
	/// Returns whether the dark current value is shown, for all able channels.
	bool darkCurrentShown() const { return darkCurrentShown_; }

signals:
	/// Notifier that the bias enabled editor visibility has changed.
	void biasEnabledEditorShownChanged(bool isShown);
	/// Notifier that the bias editor visibility has changed.
	void biasEditorShownChanged(bool isShown);
	/// Notifier that the dark current display visibility has changed.
	void darkCurrentShownChanged(bool isShown);

public slots:
	/// Sets whether the bias enabled editor is shown for all able scaler channel views.
	void setBiasEnabledEditorShown(bool shown);
	/// Sets whether the bias editor is shown for all able scaler channel views.
	void setBiasEditorShown(bool shown);
	/// Sets whether the dark current value is shown for all able scaler channel views.
	void setDarkCurrentShown(bool shown);
	/// Sets the amplifier view mode for all channels viewed.
	void setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode);
	/// Sets the channel output view mode for all channels viewed.
	void setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode newMode);

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;
	/// Bool indicating whether the bias enabled editor is shown.
	bool biasEnabledEditorShown_;
	/// Bool indicating whether the bias editor is shown.
	bool biasEditorShown_;
	/// Bool indicating whether the dark current value is shown.
	bool darkCurrentShown_;
	/// List of scaler channel views.
	QList<BioXASSIS3820ScalerChannelView*> channelViews_;

};

#endif // BIOXASSIS3820SCALERCHANNELSVIEW_H
