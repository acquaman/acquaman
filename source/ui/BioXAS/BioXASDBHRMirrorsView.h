#ifndef BIOXASDBHRMIRRORSVIEW_H
#define BIOXASDBHRMIRRORSVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASDBHRMirrors.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASDBHRMirrorsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASDBHRMirrorsView(BioXASDBHRMirrors *mirror, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASDBHRMirrorsView();

	/// Returns the mirrors being viewed.
	BioXASDBHRMirrors* mirrors() const { return mirrors_; }

signals:
	/// Notifier that the mirrors being viewed have changed.
	void mirrorsChanged(BioXASDBHRMirrors *newMirrors);

public slots:
	/// Sets the mirrors being viewed.
	void setMirrors(BioXASDBHRMirrors *newMirrors);

protected:
	/// The mirrors being viewed.
	BioXASDBHRMirrors *mirrors_;

	/// Editor for the mirror's m1 vertical control.
	AMExtendedControlEditor *m1Editor_;
	/// Editor for the mirror's m2 vertical control.
	AMExtendedControlEditor *m2Editor_;
	/// Editor for the mirror pitch control.
	AMExtendedControlEditor *pitchEditor_;
};

#endif // BIOXASDBHRMIRRORSVIEW_H
