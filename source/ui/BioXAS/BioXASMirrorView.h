#ifndef BIOXASMIRRORVIEW_H
#define BIOXASMIRRORVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASMirror;
class BioXASMirrorBendView;
class AMExtendedControlEditor;

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
	/// Refreshes the view.
	void refresh();
	/// Sets the mirror being viewed.
	void setMirror(BioXASMirror *newMirror);

protected slots:
	/// Updates the pitch editor.
	void updatePitchEditor();
	/// Updates the roll editor.
	void updateRollEditor();
	/// Updates the yaw editor.
	void updateYawEditor();
	/// Updates the height editor.
	void updateHeightEditor();
	/// Updates the lateral editor.
	void updateLateralEditor();

protected:
	/// The mirror being viewed.
	BioXASMirror *mirror_;

	/// The pitch control editor.
	AMExtendedControlEditor *pitchEditor_;
	/// The roll control editor.
	AMExtendedControlEditor *rollEditor_;
	/// The yaw control editor.
	AMExtendedControlEditor *yawEditor_;
	/// The height control editor.
	AMExtendedControlEditor *heightEditor_;
	/// The lateral control editor.
	AMExtendedControlEditor *lateralEditor_;

	/// The bend view.
	BioXASMirrorBendView *bendView_;
};

#endif // BIOXASMIRRORVIEW_H
