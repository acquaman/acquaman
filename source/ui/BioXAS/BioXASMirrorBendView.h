#ifndef BIOXASMIRRORBENDVIEW_H
#define BIOXASMIRRORBENDVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASMirror.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASMirrorBendView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorBendView(BioXASMirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMirrorBendView();

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

	/// The bend control editor.
	AMExtendedControlEditor *bendEditor_;
	/// The upstream motor control editor.
	AMExtendedControlEditor *upstreamEditor_;
	/// The downstream motor control editor.
	AMExtendedControlEditor *downstreamEditor_;
};

#endif // BIOXASMIRRORBENDVIEW_H
