#ifndef VESPERSBEAMSELECTORVIEW_H
#define VESPERSBEAMSELECTORVIEW_H

#include <QWidget>

#include "beamline/VESPERS/VESPERSBeamSelector.h"

#include <QButtonGroup>
#include <QAbstractButton>
#include <QProgressBar>

/*!
  A view for the VESPERSBeamSelector model.  The view itself is simple with only four buttons and a progress bar that shows up to show the progress to the next beam.
  The current beam will be highlighted (in green for now) to show the current beam.  The progress bar will be hidden except when it is in use.
  */
class VESPERSBeamSelectorView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes a VESPERSBeamSelector as an argument.
	explicit VESPERSBeamSelectorView(VESPERSBeamSelector *beamSelector, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changing the color of the right button to show which beam your on.
	void onCurrentBeamChanged(VESPERSBeamSelector::Beam beam);

	/// Tells the model to move the given beam.
	void changeBeam(int id);

protected:
	/// Button group containing all the buttons to the different beams.
	QButtonGroup *beams_;
	/// Arbitrary button pointer to the current selected beam.
	QAbstractButton *currentBeam_;
	/// The progress bar.
	QProgressBar *progressBar_;

	/// Pointer to the beam selector.
	VESPERSBeamSelector *beamSelector_;
};

#endif // VESPERSBEAMSELECTORVIEW_H
