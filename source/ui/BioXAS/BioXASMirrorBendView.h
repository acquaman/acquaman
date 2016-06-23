#ifndef BIOXASMIRRORBENDVIEW_H
#define BIOXASMIRRORBENDVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASMirror.h"

class AMExtendedControlEditor;
class CLSControlEditor;

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

protected slots:
	/// Updates the bend editor.
	void updateBendEditor();
	/// Updates the upstream bender editor.
	void updateUpstreamEditor();
	/// Updates the downstream bender editor.
	void updateDownstreamEditor();

protected:
	/// The mirror being viewed.
	BioXASMirror *mirror_;

	/// The bend control editor.
	CLSControlEditor *bendEditor_;
	/// The upstream motor control editor.
	CLSControlEditor *upstreamEditor_;
	/// The downstream motor control editor.
	CLSControlEditor *downstreamEditor_;
};

#endif // BIOXASMIRRORBENDVIEW_H
