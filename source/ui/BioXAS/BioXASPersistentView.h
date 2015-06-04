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

class BioXASPersistentView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASPersistentView(AMControl *energyControl = 0, BioXASSSRLMonochromatorRegionControl *regionControl = 0, AMControl *braggControl = 0, CLSSIS3820Scaler *scaler = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASPersistentView();

public slots:
	/// Sets the visibility of the scaler channels view.
	void setScalerChannelsVisible(bool isVisible);

protected:
	/// Editor that selects the mono energy.
	AMExtendedControlEditor *energyControlEditor_;
	/// Editor that selects the mono region.
	BioXASSSRLMonochromatorRegionControlEditor *regionControlEditor_;
	/// Editor that selects the mono bragg position.
	AMExtendedControlEditor *braggControlEditor_;
	/// The scaler channel views for the i0, iT, and i2 channels.
	QGroupBox *channelsBox_;

};

#endif // BIOXASPERSISTENTVIEW_H
