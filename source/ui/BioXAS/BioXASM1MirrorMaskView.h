#ifndef BIOXASM1MIRRORMASKVIEW_H
#define BIOXASM1MIRRORMASKVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASM1Mirror.h"
#include "ui/CLS/CLSControlEditor.h"

class BioXASM1MirrorMaskView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM1MirrorMaskView(BioXASM1Mirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASM1MirrorMaskView();

	/// Returns the mirror being viewed.
	BioXASM1Mirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being viewed has changed.
	void mirrorChanged(BioXASM1Mirror *newMirror);

public slots:
	/// Sets the mirror being viewed.
	void setMirror(BioXASM1Mirror *newMirror);

protected slots:
	/// Updates the mask upper blade editor.
	void updateUpperBladeEditor();
	/// Updates the mask state editor.
	void updateStateEditor();

protected:
	/// The mirror being viewed.
	BioXASM1Mirror *mirror_;

	/// The mask upper blade editor.
	CLSControlEditor *upperBladeEditor_;
	/// The mask state editor.
	CLSControlEditor *stateEditor_;
};

#endif // BIOXASM1MIRRORMASKVIEW_H
