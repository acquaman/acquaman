#ifndef BIOXASMIRROREDITOR_H
#define BIOXASMIRROREDITOR_H

#include <QWidget>
#include <QPushButton>

class BioXASMirror;
class BioXASValueSetpointEditor;

class BioXASMirrorEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorEditor(BioXASMirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorEditor();

	/// Returns the mirror being edited.
	BioXASMirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being edited has changed.
	void mirrorChanged(BioXASMirror *newMirror);

public slots:
	/// Sets the mirror being edited.
	void setMirror(BioXASMirror *newMirror);

protected slots:
	/// Updates the pitch editor.
	void updatePitchEditor();
	/// Updates the roll editor.
	void updateRollEditor();
	/// Updates the height editor.
	void updateHeightEditor();
	/// Updates the yaw editor.
	void updateYawEditor();
	/// Updates the lateral editor.
	void updateLateralEditor();

	/// Updates the move button.
	void updateMoveButton();
	/// Updates the stop button.
	void updateStopButton();

	/// Handles moving the mirror when the move button is clicked.
	void onMoveButtonClicked();
	/// Handles stopping the mirror when the stop button is clicked.
	void onStopButtonClicked();

protected:
	/// The mirror being edited.
	BioXASMirror *mirror_;

	/// The pitch setpoint editor.
	BioXASValueSetpointEditor *pitchEditor_;
	/// The roll setpoint editor.
	BioXASValueSetpointEditor *rollEditor_;
	/// The height setpoint editor.
	BioXASValueSetpointEditor *heightEditor_;
	/// The yaw setpoint editor.
	BioXASValueSetpointEditor *yawEditor_;
	/// The lateral setpoint editor.
	BioXASValueSetpointEditor *lateralEditor_;

	/// The move button.
	QPushButton *moveButton_;
	/// The stop button.
	QPushButton *stopButton_;
};

#endif // BIOXASMIRROREDITOR_H
