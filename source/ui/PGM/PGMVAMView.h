#ifndef PGMVAMVIEW_H
#define PGMVAMVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/PGM/PGMVAM.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class PGMVAMView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit PGMVAMView(PGMVAM *vam, QWidget *parent = 0);
	/// Destructor.
	virtual ~PGMVAMView();

	/// Returns the mask being viewed.
	PGMVAM* vam() const { return vam_; }

signals:
	/// Notifier that the mask being viewed has changed.
	void vamChanged(PGMVAM *newMask);

public slots:
	/// Sets the mask being viewed.
	void setVAM(PGMVAM *newMask);

protected slots:
	/// Updates the blade editors.
	void updateBladeEditors();

protected:
	/// The mask being viewed.
	PGMVAM *vam_;

	/// The upper blade editor.
	AMExtendedControlEditor *upperBladeEditor_;
	/// The lower blade editor.
	AMExtendedControlEditor *lowerBladeEditor_;
	/// The outboard blade editor.
	AMExtendedControlEditor *outboardBladeEditor_;
	/// The inboard blade editor.
	AMExtendedControlEditor *inboardBladeEditor_;
};

#endif // PGMVAMVIEW_H
