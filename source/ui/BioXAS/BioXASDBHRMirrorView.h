#ifndef BIOXASDBHRMIRRORVIEW_H
#define BIOXASDBHRMIRRORVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASDBHRMirror.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASDBHRMirrorView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASDBHRMirrorView(BioXASDBHRMirror *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASDBHRMirrorView();

	/// Returns the mirror being viewed.
	BioXASDBHRMirror* mirror() const { return mirror_; }

signals:
	/// Notifier that the mirror being viewed has changed.
	void mirrorChanged(BioXASDBHRMirror *newMirror);

public slots:
	/// Sets the mirror being viewed.
	void setMirror(BioXASDBHRMirror *newMirror);

protected:
	/// The mirror being viewed.
	BioXASDBHRMirror *mirror_;

	/// The main layout.
	QVBoxLayout *layout_;
	/// Editor for the mirror pitch control.
	AMExtendedControlEditor *pitchEditor_;
	/// Editor for the mirror's m1 vertical control.
	AMExtendedControlEditor *m1Editor_;
	/// Editor for the mirror's m2 vertical control.
	AMExtendedControlEditor *m2Editor_;

};

#endif // BIOXASDBHRMIRRORVIEW_H
