#ifndef VESPERSBEAMSELECTORVIEW_H
#define VESPERSBEAMSELECTORVIEW_H

#include <QWidget>

#include <QButtonGroup>
#include <QAbstractButton>
#include <QProgressBar>

#include "beamline/VESPERS/VESPERSBeamline.h"

/*!
  A view for the VESPERSBeamSelector model.  The view itself is simple with only four buttons and a progress bar that shows up to show the progress to the next beam.
  The current beam will be highlighted (in green for now) to show the current beam.  The progress bar will be hidden except when it is in use.
  */
class VESPERSBeamSelectorView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes a VESPERSBeamSelector as an argument.
	explicit VESPERSBeamSelectorView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changing the color of the right button to show which beam your on.
	void onCurrentBeamChanged(VESPERSBeamline::Beam beam);

	/// Tells the model to move the given beam.
	void changeBeam(int id);

	/// Handles the clean up after changing beams.
	void onBeamChangeCompleted() { progressBar_->hide(); }

	/// Handles updating the progress bar as the motor is moving.
	void onProgressUpdate(double current, double end);

protected:
	/// Button group containing all the buttons to the different beams.
	QButtonGroup *beams_;
	/// Arbitrary button pointer to the current selected beam.
	QAbstractButton *currentBeam_;
	/// The progress bar.
	QProgressBar *progressBar_;
};

#endif // VESPERSBEAMSELECTORVIEW_H
