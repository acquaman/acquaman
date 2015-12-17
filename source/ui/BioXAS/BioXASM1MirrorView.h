#ifndef BIOXASM1MIRRORVIEW_H
#define BIOXASM1MIRRORVIEW_H

#include <QWidget>

#include "ui/BioXAS/BioXASMirrorView.h"
#include "beamline/BioXAS/BioXASM1Mirror.h"
#include "ui/BioXAS/BioXASMirrorBendView.h"
#include "ui/BioXAS/BioXASM1MirrorMaskView.h"
#include "ui/beamline/AMControlStopButton.h"

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
	/// Refreshes the view.
	void refresh();
	/// Sets the mirror being viewed.
	void setMirror(BioXASM1Mirror *newMirror);

protected slots:
	/// Updates the mask view.
	void updateMaskView();

protected:
	/// The mirror being viewed.
	BioXASM1Mirror *mirror_;

	/// The stop button.
	AMControlStopButton *stopButton_;
	/// The mirror mask view.
	BioXASM1MirrorMaskView *maskView_;
	/// The basic mirror view.
	BioXASMirrorView *mirrorView_;
	/// The mirror bend view.
	BioXASMirrorBendView *bendView_;
};

#endif // BIOXASM1MIRRORVIEW_H
