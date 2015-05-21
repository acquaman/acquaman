#ifndef BIOXASMIRRORVIEW_H
#define BIOXASMIRRORVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASMirror.h"
#include "ui/beamline/AMExtendedControlEditor.h"

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

protected:
	/// The mirror being viewed.
	BioXASMirror *mirror_;

	/// The upstream inboard control editor.
	AMExtendedControlEditor *upstreamInboardEditor_;
	/// The upstream outboard control editor.
	AMExtendedControlEditor *upstreamOutboardEditor_;
	/// The downstream control editor.
	AMExtendedControlEditor *downstreamEditor_;
	/// The pitch control editor.
	AMExtendedControlEditor *pitchEditor_;
};

#endif // BIOXASMIRRORVIEW_H
