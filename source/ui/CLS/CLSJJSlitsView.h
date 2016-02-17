#ifndef CLSJJSLITSVIEW_H
#define CLSJJSLITSVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

#include "beamline/CLS/CLSJJSlits.h"
#include "ui/beamline/AMSlitView.h"

class CLSJJSlitsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitsView(CLSJJSlits *slits, bool showBladeEditors = true, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitsView();

	/// Returns the slits being viewed.
	CLSJJSlits* control() const { return slits_; }

signals:
	/// Notifier that the JJ slits being viewed have changed.
	void controlChanged(CLSJJSlits *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the slits.
	void setControl(CLSJJSlits *newControl);
	/// Sets the flag for whether the slit blade editors are shown.
	void setBladeEditorsShown(bool showEditors);

protected:
	/// The slits being viewed.
	CLSJJSlits *slits_;

	/// The vertical slit view.
	AMSlitView *verticalSlitView_;
	/// The horizontal slit view.
	AMSlitView *horizontalSlitView_;
};

#endif // CLSJJSLITSVIEW_H
