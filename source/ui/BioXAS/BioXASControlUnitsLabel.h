#ifndef BIOXASCONTROLUNITSLABEL_H
#define BIOXASCONTROLUNITSLABEL_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

class AMControl;

class BioXASControlUnitsLabel : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlUnitsLabel(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlUnitsLabel();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected slots:
	/// Clears the displayed units.
	void clear();
	/// Updates the displayed units.
	void update();

protected:
	/// The control being viewed.
	AMControl *control_;
	/// The units label.
	QLabel *unitsLabel_;
};

#endif // BIOXASCONTROLUNITSLABEL_H
