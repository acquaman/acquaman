#ifndef AMSLITSVIEW_H
#define AMSLITSVIEW_H

#include <QWidget>

class AMSlits;
class AMSlitView;

class AMSlitsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlitsView(AMSlits *slits, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMSlitsView();

	/// Returns the slits being viewed.
	AMSlits* control() const { return slits_; }

signals:
	/// Notifier that the JJ slits being viewed have changed.
	void controlChanged(AMSlits *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the slits.
	void setControl(AMSlits *newControl);

protected:
	/// The slits being viewed.
	AMSlits *slits_;

	/// The vertical slit view.
	AMSlitView *verticalSlitView_;
	/// The horizontal slit view.
	AMSlitView *horizontalSlitView_;
};

#endif // AMSLITSVIEW_H
