#ifndef PGMVARIABLEAPERTUREMASKVIEW_H
#define PGMVARIABLEAPERTUREMASKVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/PGM/PGMVariableApertureMask.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class PGMVariableApertureMaskView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit PGMVariableApertureMaskView(PGMVariableApertureMask *vam, QWidget *parent = 0);
	/// Destructor.
	virtual ~PGMVariableApertureMaskView();

	/// Returns the mask being viewed.
	PGMVariableApertureMask* vam() const { return vam_; }

signals:
	/// Notifier that the mask being viewed has changed.
	void vamChanged(PGMVariableApertureMask *newMask);

public slots:
	/// Sets the mask being viewed.
	void setVAM(PGMVariableApertureMask *newMask);

protected slots:
	/// Updates the blade editors.
	void updateBladeEditors();

protected:
	/// The mask being viewed.
	PGMVariableApertureMask *vam_;

	/// The upper blade editor.
	AMExtendedControlEditor *upperBladeEditor_;
	/// The lower blade editor.
	AMExtendedControlEditor *lowerBladeEditor_;
	/// The outboard blade editor.
	AMExtendedControlEditor *outboardBladeEditor_;
	/// The inboard blade editor.
	AMExtendedControlEditor *inboardBladeEditor_;
};

#endif // PGMVARIABLEAPERTUREMASKVIEW_H
