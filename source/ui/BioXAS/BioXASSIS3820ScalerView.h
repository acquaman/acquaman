#ifndef BIOXASSIS3820SCALERVIEW_H
#define BIOXASSIS3820SCALERVIEW_H

#include <QWidget>
#include <QGroupBox>

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/CLS/CLSSIS3820ScalerControlsView.h"

class BioXASSIS3820ScalerView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerView();
	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

signals:

public slots:

protected slots:
	/// Handles showing or hiding the scaler channels view, in response to a change in the scaler's connection state.
	void onScalerConnectedChanged();

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;

	/// The scaler controls view.
	CLSSIS3820ScalerControlsView *controlsView_;
	/// The scaler channels view.
	QGroupBox *channelsView_;

};

#endif // BIOXASSIS3820SCALERVIEW_H
