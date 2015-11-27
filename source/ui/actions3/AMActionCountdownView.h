#ifndef AMACTIONREMAININGTIMEVIEW_H
#define AMACTIONREMAININGTIMEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "actions3/AMAction3.h"

class AMActionCountdownView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMActionCountdownView(AMAction3 *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMActionCountdownView();

	/// Returns the action being viewed.
	AMAction3* action() const { return action_; }

signals:
	/// Notifier that the action being viewed has changed.
	void actionChanged(AMAction3 *newAction);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();
	/// Sets the action being viewed.
	void setAction(AMAction3 *newAction);

protected slots:
	/// Sets the action countdown being viewed.
	void setValue(int newCount);

	/// Updates the countdown label.
	void updateCountdownLabel();

	/// Handles invalidating the current action when it has reached a final state.
	void onActionStateChanged(int oldState, int newState);
	/// Handles updating the current count value when the action reports a change in progress.
	void onActionProgressChanged(double currentValue, double totalValue);

protected:
	/// Returns the progress remaining from the given progress records.
	double progressRemaining(double currentProgress, double totalProgress);

protected:
	/// The action being viewed.
	AMAction3 *action_;
	/// The action countdown value being viewed.
	int value_;

	/// The label displaying the progress countdown.
	QLabel *countdownLabel_;
};

#endif // AMACTIONREMAININGTIMEVIEW_H
