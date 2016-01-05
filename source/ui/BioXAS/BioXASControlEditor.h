#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QAction>
#include <QProgressBar>

class AMControl;

class BioXASControlEditor : public QGroupBox
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlEditor(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlEditor();

	/// Returns the control being edited.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the editor.
	void refresh();
	/// Sets the control being edited.
	void setControl(AMControl *newControl);

protected slots:
	/// Updates the value bar.
	void updateValueLabel();

	/// Updates the move action.
	void updateMoveAction();
	/// Updates the stop action.
	void updateStopAction();
	/// Updates the calibrate action.
	void updateCalibrateAction();

	/// Handles displaying the custom context menu, visible when editor is right-clicked.
	void onContextMenuRequested(const QPoint &clickPosition);

	/// Handles initiating a control move when the move action is triggered.
	void onMoveActionTriggered() {}
	/// Handles initiating a control stop when the stop action is triggered.
	void onStopActionTriggered() {}
	/// Handles initiating a control calibration when the calibrate action is triggered.
	void onCalibrateActionTriggered() {}

protected:
	/// The control being edited.
	AMControl *control_;

	/// The value label.
	QLabel *valueLabel_;
	/// The move  progress bar.
	QProgressBar *moveProgressBar_;

	/// The move action.
	QAction *moveAction_;
	/// The stop action.
	QAction *stopAction_;
	/// The calibrate action.
	QAction *calibrateAction_;
};

#endif // BIOXASCONTROLEDITOR_H
