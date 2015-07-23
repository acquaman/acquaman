#ifndef BIOXASPERSISTENTVIEW_H
#define BIOXASPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

class AMExtendedControlEditor;
class BioXASSIS3820ScalerChannelsView;
class BioXASSSRLMonochromatorRegionControl;
class BioXASSSRLMonochromatorRegionControlEditor;
class AMControl;
class CLSSIS3820Scaler;
class BioXASSSRLMonochromator;

class BioXASPersistentView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASPersistentView(BioXASSSRLMonochromator *mono = 0, CLSSIS3820Scaler *scaler = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASPersistentView();
	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }
	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);
	/// Notifier that the scaler being viewed has changed.
	void scalerChanged(CLSSIS3820Scaler *newScaler);

public slots:
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);
	/// Sets the visibility of the scaler channels view.
	void setScalerChannelsVisible(bool isVisible);

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;

	/// Display that edits the step-based energy.
	AMExtendedControlEditor *energyEditor_;
	/// Editor that selects the mono region.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
	/// Display that edits the bragg step-based position.
	AMExtendedControlEditor *braggEditor_;
	/// The scaler channel views for the i0, iT, and i2 channels.
	QGroupBox *channelsBox_;
};

#endif // BIOXASPERSISTENTVIEW_H
