#ifndef BIOXASM1MIRRORVIEW_H
#define BIOXASM1MIRRORVIEW_H

#include <QWidget>
#include <QGroupBox>

#include "ui/BioXAS/BioXASMirrorView.h"
#include "ui/CLS/CLSControlEditor.h"
#include "beamline/BioXAS/BioXASM1Mirror.h"

class BioXASM1MirrorView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM1MirrorView(BioXASM1Mirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASM1MirrorView();

	/// Returns the mirror being viewed.
	BioXASM1Mirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being viewed has changed.
	void mirrorChanged(BioXASM1Mirror *newMirror);

public slots:
	/// Sets the mirror being viewed.
	void setMirror(BioXASM1Mirror *newMirror);

protected slots:
	/// Updates the mask view.
	void updateMaskBladeEditor();
	/// Updates the basic mirror view.
	void updateBasicView();
	/// Updates the downstream blade current editor.
	void updateDownstreamBladeCurrentEditor();

protected:
	/// The mirror being viewed.
	BioXASM1Mirror *mirror_;

	/// The mask blade editor.
	CLSControlEditor *maskBladeEditor_;
	/// The basic mirror view.
	BioXASMirrorView *basicView_;
	/// The downstream blade current editor.
	CLSControlEditor *downstreamBladeCurrentEditor_;
};

#endif // BIOXASM1MIRRORVIEW_H
