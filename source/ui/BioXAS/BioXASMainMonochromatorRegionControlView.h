#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QProgressBar>

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
	/// Handles updating the view when a region move has started.
	void onMoveStarted();
	/// Handles updating the view when a region move has failed.
	void onMoveFailed(int failureCode);
	/// Handles updating the view when a region move has succeeded.
	void onMoveSucceeded();

protected:
	/// The region control being viewed.
	BioXASMainMonochromatorRegionControl *regionControl_;
	/// The view displayed when the region control is moving as a result of this software.
	QWidget *movingView_;
	/// The progress bar in the moving view, displays the progress made through a region change.
	/// The label in the moving view that describes the current step being performed.
	QLabel *movingDescription_;
	/// The view displayed when the region control reports a move has failed.
	QWidget *failedView_;
	/// The label in the failed view that provides the failure explaination.
	QLabel *failedDescription_;
	/// The view displayed when the region control reports a move has succeeded.
	QWidget *succeededView_;

private:
	/// Shows the moving view, hides the other views.
	void showMovingView();
	/// Shows the failed view, hides the other views.
	void showFailedView(const QString &message);
	/// Shows the succeeded view, hides the other views.
	void showSucceededView();
};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROLVIEW_H
