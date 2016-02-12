#ifndef CLSJJSLITSVIEW_H
#define CLSJJSLITSVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>

class CLSJJSlits;
class AMExtendedControlEditor;

class CLSJJSlitsView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor
	explicit CLSJJSlitsView(CLSJJSlits *jjSlit, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitsView();

	/// Returns the JJ slits being viewed.
	CLSJJSlits* control() const { return jjSlits_; }

signals:
	/// Notifier that the JJ slits being viewed have changed.
	void controlChanged(CLSJJSlits *newSlits);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the JJ slits being viewed.
	void setControl(CLSJJSlits *newSlits);
	/// Sets the dataRange of the slits view
	void setDataRange(double maxValue, double minValue);

protected:
	/// The JJ slits being viewed.
	CLSJJSlits *jjSlits_;

	/// The gap editor for the vertical blades.
	AMExtendedControlEditor *verticalGapEditor_;
	/// The center editor for the vertical blades.
	AMExtendedControlEditor *verticalCenterEditor_;
	/// The gap editor for the horizontal blades.
	AMExtendedControlEditor *horizontalGapEditor_;
	/// The center editor for the horizontal blades.
	AMExtendedControlEditor *horizontalCenterEditor_;
};

#endif // CLSJJSLITSVIEW_H
