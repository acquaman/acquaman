#ifndef BIOXASSIS3820SCALERCHANNELVIEW_H
#define BIOXASSIS3820SCALERCHANNELVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"

class BioXASSIS3820ScalerChannelView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerChannelView();

	/// Returns the channel being viewed.
	CLSSIS3820ScalerChannel* channel() const { return channel_; }

signals:

public slots:

protected:
	/// The scaler channel being viewed.
	CLSSIS3820ScalerChannel *channel_;
};

#endif // BIOXASSIS3820SCALERCHANNELVIEW_H
