#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusView.h"


class BioXASBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusView(BioXASFrontEndBeamStatusControl *frontEndBeamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the front-end beam status control.
	void setFrontEndBeamStatusControl(BioXASFrontEndBeamStatusControl *newControl);

protected:
	/// The front-end beam status view.
	BioXASFrontEndBeamStatusView *frontEndBeamStatusView_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
