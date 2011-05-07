#ifndef VESPERSPERSISTENTVIEW_H
#define VESPERSPERSISTENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "beamline/VESPERS/VESPERSBeamline.h"

class VESPERSPersistentView : public QWidget
{
	Q_OBJECT
public:
	/// Default constructor.
	explicit VESPERSPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the valves button push.  Opens or closes based the current state of the current state.
	void onValvesButtonPushed();
	/// Handles when the valves state changes.
	void onValvesStateChanged();

protected:
	/// Button and label for the valves.
	QPushButton *valvesButton_;
	QLabel *valvesStatus_;

	/// The valve control.
	VESPERSValveGroupControl *valves_;
};

#endif // VESPERSPERSISTENTVIEW_H
