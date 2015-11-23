#ifndef SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
#define SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

#include "actions3/SGM/SGMSampleChamberVacuumMoveAction.h"

class SGMSampleChamberVacuumMoveActionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumMoveActionView(SGMSampleChamberVacuumMoveAction *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveActionView();

	/// Returns the action being viewed.
	SGMSampleChamberVacuumMoveAction* action() const { return action_; }

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(SGMSampleChamberVacuumMoveAction *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action being viewed.
	void setAction(SGMSampleChamberVacuumMoveAction *newAction);

protected:
	/// The action being viewed.
	SGMSampleChamberVacuumMoveAction *action_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEACTIONVIEW_H
