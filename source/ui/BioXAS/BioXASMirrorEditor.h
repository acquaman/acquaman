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

public slots:

protected:
	/// The pitch setpoint editor.
	BioXASValueSetpointEditor *pitchEditor_;
	/// The roll setpoint editor.
	BioXASValueSetpointEditor *roll_;
	/// The height setpoint editor.
	BioXASValueSetpointEditor *height_;
	/// The yaw setpoint editor.
	BioXASValueSetpointEditor *yawEditor_;
	/// The lateral setpoint editor.
	BioXASValueSetpointEditor *lateralEditor_;

	/// The move button.
	QPushButton *moveButton_;
	/// The stop button.
	QPushButton *stop_;
};

#endif // BIOXASMIRROREDITOR_H
