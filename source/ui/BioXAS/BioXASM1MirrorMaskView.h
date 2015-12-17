#ifndef BIOXASM1MIRRORMASKVIEW_H
#define BIOXASM1MIRRORMASKVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;
class BioXASM1MirrorMask;

class BioXASM1MirrorMaskView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM1MirrorMaskView(BioXASM1MirrorMask *mask, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASM1MirrorMaskView();

	/// Returns the mirror mask being viewed.
	BioXASM1MirrorMask* mask() const { return mask_; }

signals:
	/// Notifier that the mirror mask being viewed has changed.
	void maskChanged(BioXASM1MirrorMask *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mirror mask being viewed.
	void setMirrorMask(BioXASM1MirrorMask *newControl);

protected slots:
	/// Updates the upper blade editor.
	void updateUpperBladeEditor();
	/// Updates the state editor.
	void updateStateEditor();

protected:
	/// The mask being viewed.
	BioXASM1MirrorMask *mask_;

	/// The upper blade editor.
	AMExtendedControlEditor *upperBladeEditor_;
	/// The state editor.
	AMExtendedControlEditor *stateEditor_;
};

#endif // BIOXASM1MIRRORMASKVIEW_H
