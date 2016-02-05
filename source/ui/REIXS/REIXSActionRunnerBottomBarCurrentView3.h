#ifndef REIXSACTIONRUNNERBOTTOMBARCURRENTVIEW3_H
#define REIXSACTIONRUNNERBOTTOMBARCURRENTVIEW3_H

#include "source/ui/actions3/AMActionRunnerBottomBarCurrentView3.h"

class REIXSActionRunnerBottomBarCurrentView3 : public AMActionRunnerBottomBarCurrentView3
{
    Q_OBJECT
public:
	explicit REIXSActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When the current action in the AMActionRunner changes
	virtual void onCurrentActionChanged(AMAction3 *action);
	/// Displays dialog to extend the current XES scan duration
	void onAddXESScanTimeClicked();

protected:
	/// The button to modify the XES scan time
	QToolButton *addXESScanTime_;
	/// Pointer to the running action (if it exists).
	AMAction3 *action_;



};

#endif // REIXSACTIONRUNNERBOTTOMBARCURRENTVIEW3_H
