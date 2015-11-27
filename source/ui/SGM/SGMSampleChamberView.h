#ifndef SGMSAMPLECHAMBERVIEW_H
#define SGMSAMPLECHAMBERVIEW_H

#include <QPushButton>

#include "ui/SGM/SGMSampleChamberVacuumView.h"
#include "beamline/SGM/SGMSampleChamber.h"

class SGMSampleChamberView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberView(SGMSampleChamber *sampleChamber, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberView();

	/// Returns the sample chamber being viewed.
	SGMSampleChamber* sampleChamber() const { return sampleChamber_; }

signals:
	/// Notifier that the sample chamber being viewed has changed.
	void sampleChamberChanged(SGMSampleChamber *newSampleChamber);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the sample chamber being viewed.
	void setSampleChamber(SGMSampleChamber *newSampleChamber);

protected slots:
	/// Updates the sample change button.
	void updateSampleChangeButton();
	/// Updates the vacuum view.
	void updateVacuumView();

	/// Handles initiating the sample change process when the sample change button is clicked.
	void onSampleChangeButtonClicked();

protected:
	/// The sample chamber being viewed.
	SGMSampleChamber *sampleChamber_;

	/// The change sample button.
	QPushButton *sampleChangeButton_;
	/// The vacuum view.
	SGMSampleChamberVacuumView *vacuumView_;
};

#endif // SGMSAMPLECHAMBERVIEW_H
