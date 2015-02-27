#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include "beamline/BioXAS/BioXASMainMonochromatorRegionControl.h"

class BioXASMainMonochromatorRegionControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorRegionControlView(BioXASMainMonochromatorRegionControl *regionControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionControlView();
	/// Returns the region control being viewed.
	AMControl* regionControl() const { return regionControl_; }

signals:
	/// Notifier that the region control being viewed has changed.
	void regionControlChanged(AMControl *newControl);

public slots:
	/// Sets the region control being viewed.
	void setRegionControl(BioXASMainMonochromatorRegionControl *newControl);

protected slots:
	/// Handles proceeding with the crystal/region change if the 'OK' button in the Initialize View is clicked.
	void onInitializeViewAccepted();

protected:
	/// The region control being viewed.
	BioXASMainMonochromatorRegionControl *regionControl_;
	/// The initialize view.
	QWidget *initializeView_;
};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
