#ifndef BIOXASM2MIRRORVIEW_H
#define BIOXASM2MIRRORVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASM2Mirror.h"

class AMExtendedControlEditor;

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
	/// Sets the mirror being viewed.
	void setMirror(BioXASM2Mirror *newMirror);

protected:
	/// The mirror being viewed.
	BioXASM2Mirror *mirror_;

	/// The main layout.
	QVBoxLayout *layout_;
	/// The editor for the screen control.
	AMExtendedControlEditor *screenEditor_;

};

#endif // BIOXASM2MIRRORVIEW_H
