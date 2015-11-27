#ifndef AMACTIONSTATUSVIEW_H
#define AMACTIONSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "actions3/AMAction3.h"

class AMActionStatusView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMActionStatusView(AMAction3 *action, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMActionStatusView();

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
	/// Updates the status label.
	void updateStatusLabel();

protected:
	/// The action being viewed.
	AMAction3 *action_;

	/// The status label.
	QLabel *statusLabel_;
};

#endif // AMACTIONSTATUSVIEW_H
