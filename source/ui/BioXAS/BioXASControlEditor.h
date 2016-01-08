#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QApplication>

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

	/// Returns whether using the control name as the editor title.
	bool useControlNameAsTitle() const { return useControlNameAsTitle_; }
	/// Returns the value format.
	QChar format() const { return format_; }
	/// Returns the value precision.
	int precision() const { return precision_; }
	/// Returns the units.
	QString units() const { return units_; }
	/// Returns whether using the control units as the editor units.
	bool useControlUnitsAsUnits() const { return useControlUnitsAsUnits_; }
	/// Returns the current read-only status of the editor.
	bool readOnly() const { return readOnly_; }

signals:
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the title text has changed.
	void titleChanged(const QString &newText);
	/// Notifier that the flag indicating whether the control's name is used as the title text has changed.
	void useControlNameAsTitleChanged(bool useName);
	/// Notifier that the value format has changed.
	void formatChanged(const QChar &newFormat);
	/// Notifier that the value precision has changed.
	void precisionChanged(int newPrecision);
	/// Notifier that the units text has changed.
	void unitsChanged(const QString &newUnits);
	/// Notifier that the flag indicating whether the control's units are used as the editor's units has changed.
	void useControlUnitsAsUnitsChanged(bool useUnits);
	/// Notifier that the read-only status of the editor has changed.
	void readOnlyChanged(bool readOnly);

public slots:
	/// Refreshes the editor.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

	/// Sets the editor title text.
	void setTitle(const QString &title);
	/// Sets whether the control's name is used for the title text.
	void setUseControlNameAsTitle(bool useName);
	/// Sets the value format.
	void setFormat(const QChar &newFormat);
	/// Sets the value precision.
	void setPrecision(int newPrecision);
	/// Sets the units text.
	void setUnits(const QString &newUnits);
	/// Sets whether the control's units are used for the units text.
	void setUseControlUnitsAsUnits(bool useUnits);
	/// Sets whether the editor is read-only.
	void setReadOnly(bool readOnly);

protected slots:
	/// Updates the title.
	void updateTitle();
	/// Updates the value label.
	void updateValueLabel();

	/// Sets the title text.
	void setTitleText(const QString &newText);

	/// Updates the actions.
	void updateActions();
	/// Updates the move action.
	void updateMoveAction();
	/// Updates the stop action.
	void updateStopAction();
	/// Updates the calibrate action.
	void updateCalibrateAction();

	/// Handles initiating a move, when the move action is triggered.
	void onMoveActionTriggered();
	/// Handles initiating a stop, when the stop action is triggered.
	void onStopActionTriggered();
	/// Handles initiating a calibration, when the calibrate action is triggered.
	void onCalibrateActionTriggered();

	/// Handles displaying context menu options when requested.
	void onContextMenuRequested(const QPoint &clickPosition);

protected:
	/// Handle the mouse release event, allowing the user to begin editing a value.
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	/// The control being edited.
	AMControl* control_;
	/// The title text.
	QString title_;
	/// Flag indicating whether to use the control's name as the title text.
	bool useControlNameAsTitle_;
	/// The value format.
	QChar format_;
	/// The value precision.
	int precision_;
	/// The units text.
	QString units_;
	/// Flag indicating whether to use the control's units as the units text.
	bool useControlUnitsAsUnits_;
	/// The read-only status.
	bool readOnly_;
	/// Flag indicating whether the border is visible.
	bool borderVisible_;

	/// The value label.
	QLabel* valueLabel_;

	/// The move action.
	QAction *moveAction_;
	/// The stop action.
	QAction *stopAction_;
	/// The calibrate action.
	QAction *calibrateAction_;
};

#endif // BIOXASCONTROLEDITOR_H
