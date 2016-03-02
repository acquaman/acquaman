#ifndef AMSLITSVIEW_H
#define AMSLITSVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

#include "beamline/AMSlits.h"
#include "ui/beamline/AMSlitView.h"

class AMSlitsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlitsView(AMSlits *slits, bool showBladeEditors = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMSlitsView();

	/// Returns the slits being viewed.
	AMSlits* control() const { return slits_; }

signals:
	/// Notifier that the slits being viewed have changed.
	void controlChanged(AMSlits *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the slits.
	void setControl(AMSlits *newControl);
	/// Sets the flag for whether the slit blade editors are shown.
	void setBladeEditorsShown(bool showEditors);

protected:
	/// The slits being viewed.
	AMSlits *slits_;

	/// The vertical slit view.
	AMSlitView *verticalSlitView_;
	/// The horizontal slit view.
	AMSlitView *horizontalSlitView_;
};

#endif // AMSLITSVIEW_H
