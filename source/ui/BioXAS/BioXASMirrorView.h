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
	/// Refreshes the view.
	void refresh();
	/// Sets the mirror being viewed.
	void setMirror(BioXASMirror *newMirror);

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
};

#endif // BIOXASMIRRORVIEW_H
