#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QApplication>

#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASControlEditor : public AMExtendedControlEditor
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlEditor(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlEditor();

protected slots:
	/// Updates the actions.
	void updateActions();
	/// Updates the move action.
	void updateMoveAction();
	/// Updates the stop action.
	void updateStopAction();
	/// Updates the calibrate action.
	void updateCalibrateAction();

	/// Handles initiating a move, when the move action is triggered.
	void onMoveActionTriggered();
	/// Handles initiating a stop, when the stop action is triggered.
	void onStopActionTriggered();
	/// Handles initiating a calibration, when the calibrate action is triggered.
	void onCalibrateActionTriggered();

	/// Handles displaying context menu options when requested.
	void onContextMenuRequested(const QPoint &clickPosition);

protected:
	/// The move action.
	QAction *moveAction_;
	/// The stop action.
	QAction *stopAction_;
	/// The calibrate action.
	QAction *calibrateAction_;
};

#endif // BIOXASCONTROLEDITOR_H
