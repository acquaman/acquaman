#ifndef BIOXASMIRRORVIEW_H
#define BIOXASMIRRORVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>

#include "beamline/BioXAS/BioXASMirror.h"
#include "ui/BioXAS/BioXASControlSetpointEditor.h"

class BioXASMirrorView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorView(BioXASMirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorView();

	/// Returns the mirror being viewed.
	BioXASMirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being viewed has changed.
	void mirrorChanged(BioXASMirror *newMirror);

public slots:
	/// Sets the mirror being viewed.
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
	/// Updates the bend editor.
	void updateBendEditor();

	/// Updates the move button.
	void updateMoveButton();
	/// Updates the stop button.
	void updateStopButton();

	/// Handles moving the mirror when the move button is clicked.
	void onMoveButtonClicked();
	/// Handles stopping the mirror when the stop button is clicked.
	void onStopButtonClicked();

protected:
	/// The mirror being viewed.
	BioXASMirror *mirror_;

	/// The pitch control editor.
	BioXASControlSetpointEditor *pitchEditor_;
	/// The roll control editor.
	BioXASControlSetpointEditor *rollEditor_;
	/// The height control editor.
	BioXASControlSetpointEditor *heightEditor_;
	/// The yaw control editor.
	BioXASControlSetpointEditor *yawEditor_;
	/// The lateral control editor.
	BioXASControlSetpointEditor *lateralEditor_;
	/// The bend control editor.
	BioXASControlSetpointEditor *bendEditor_;

	/// The move button.
	QPushButton *moveButton_;
	/// The stop button.
	QPushButton *stopButton_;
};

#endif // BIOXASMIRRORVIEW_H
