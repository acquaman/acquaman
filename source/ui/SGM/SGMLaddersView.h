#ifndef SGMLADDERSVIEW_H
#define SGMLADDERSVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/SGM/SGMXPSLadder.h"
#include "beamline/SGM/SGMBypassLadder.h"
#include "beamline/SGM/SGMXASLadder.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class SGMLaddersView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMLaddersView(SGMXPSLadder *xpsLadder, SGMBypassLadder *bypassLadder, SGMXASLadder *xasLadder, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMLaddersView();

	/// Returns the XPS ladder being viewed.
	SGMXPSLadder* xpsLadder() const { return xpsLadder_; }
	/// Returns the bypass ladder being viewed.
	SGMBypassLadder* bypassLadder() const { return bypassLadder_; }
	/// Returns the XAS ladder being viewed.
	SGMXASLadder* xasLadder() const { return xasLadder_; }

signals:
	/// Notifier that the XPS ladder being viewed has changed.
	void xpsLadderChanged(SGMXPSLadder *newLadder);
	/// Notifier that the bypass ladder being viewed has changed.
	void bypassLadderChanged(SGMBypassLadder *newLadder);
	/// Notifier that the XAS ladder being viewed has changed.
	void xasLadderChanged(SGMXASLadder *newLadder);

public slots:
	/// Sets the XPS ladder being viewed.
	void setXPSLadder(SGMXPSLadder *newLadder);
	/// Sets the bypass ladder being viewed.
	void setBypassLadder(SGMBypassLadder *newLadder);
	/// Sets the XAS ladder being viewed.
	void setXASLadder(SGMXASLadder *newLadder);

protected:
	/// The XPS ladder being viewed.
	SGMXPSLadder *xpsLadder_;
	/// The bypass ladder being viewed.
	SGMBypassLadder *bypassLadder_;
	/// The XAS ladder being viewed.
	SGMXASLadder *xasLadder_;

	/// The XPS ladder control editor.
	AMExtendedControlEditor *xpsLadderEditor_;
	/// The bypass ladder control editor.
	AMExtendedControlEditor *bypassLadderEditor_;
	/// The XAS ladder control editor.
	AMExtendedControlEditor *xasLadderEditor_;
};

#endif // SGMLADDERSVIEW_H
