#ifndef BIOXASM2MIRRORVIEW_H
#define BIOXASM2MIRRORVIEW_H

#include "ui/BioXAS/BioXASMirrorView.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"
#include "ui/BioXAS/BioXASMirrorBendView.h"
#include "ui/beamline/AMControlStopButton.h"
#include "ui/CLS/CLSControlEditor.h"

class BioXASM2MirrorView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM2MirrorView(BioXASM2Mirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASM2MirrorView();

	/// Returns the mirror being viewed.
	BioXASM2Mirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being viewed has changed.
	void mirrorChanged(BioXASM2Mirror *newMirror);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mirror being viewed.
	void setMirror(BioXASM2Mirror *newMirror);

protected slots:
	/// Updates the screen control editor.
	void updateScreenEditor();

protected:
	/// The mirror being viewed.
	BioXASM2Mirror *mirror_;

	/// The stop button.
	AMControlStopButton *stopButton_;
	/// The basic mirror editor.
	BioXASMirrorView *mirrorView_;
	/// The editor for the screen control.
	CLSControlEditor *screenEditor_;
};

#endif // BIOXASM2MIRRORVIEW_H
