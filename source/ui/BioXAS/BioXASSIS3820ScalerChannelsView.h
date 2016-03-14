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
	explicit BioXASSIS3820ScalerChannelsView(CLSSIS3820Scaler *scaler, bool biasEnabledVisible = false, bool biasVisible = false, bool darkCurrentVisible = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerChannelsView();

	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

	/// Returns whether the bias enabled editor is shown, for all able channels.
	bool biasEnabledEditorVisible() const { return biasEnabledEditorVisible_; }
	/// Returns whether the bias editor is shown, for all able channels.
	bool biasEditorVisible() const { return biasEditorVisible_; }
	/// Returns whether the dark current value is shown, for all able channels.
	bool darkCurrentVisible() const { return darkCurrentVisible_; }

signals:
	/// Notifier that the bias enabled editor visibility has changed.
	void biasEnabledEditorVisibleChanged(bool isVisible);
	/// Notifier that the bias editor visibility has changed.
	void biasEditorVisibleChanged(bool isVisible);
	/// Notifier that the dark current display visibility has changed.
	void darkCurrentVisibleChanged(bool isVisible);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets whether the bias enabled editor is shown for all able scaler channel views.
	void setBiasEnabledEditorVisible(bool isVisible);
	/// Sets whether the bias editor is shown for all able scaler channel views.
	void setBiasEditorVisible(bool isVisible);
	/// Sets whether the dark current value is shown for all able scaler channel views.
	void setDarkCurrentVisible(bool isVisible);
	/// Sets the amplifier view mode for all channels viewed.
	void setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode);
	/// Sets the channel output view mode for all channels viewed.
	void setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode newMode);

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;
	/// Bool indicating whether the bias enabled editor is shown.
	bool biasEnabledEditorVisible_;
	/// Bool indicating whether the bias editor is shown.
	bool biasEditorVisible_;
	/// Bool indicating whether the dark current value is shown.
	bool darkCurrentVisible_;
	/// List of scaler channel views.
	QList<BioXASSIS3820ScalerChannelView*> channelViews_;
};

#endif // BIOXASSIS3820SCALERCHANNELSVIEW_H
