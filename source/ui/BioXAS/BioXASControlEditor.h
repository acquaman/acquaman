#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

class AMControl;

class BioXASControlEditor : public QGroupBox
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlEditor(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlEditor();

	/// Returns the control being edited.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the editor.
	void refresh();
	/// Sets the control being edited.
	void setControl(AMControl *newControl);

protected slots:
	/// Updates the value label.
	void updateValueLabel();
	/// Updates the units label.
	void updateUnitsLabel();

protected:
	/// The control being edited.
	AMControl *control_;

	/// The value label.
	QLabel *valueLabel_;

	/// Flag indicating whether the editor should display the control's units or the given units.
	bool displayControlUnits_;
	/// The units label.
	QLabel *unitsLabel_;
};

#endif // BIOXASCONTROLEDITOR_H
