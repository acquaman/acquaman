#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QApplication>
#include <QMouseEvent>

class AMNumber;
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

	/// Returns the title text.
	QString title() const { return title_; }
	/// Returns whether using the control name as the editor title.
	bool useControlNameAsTitle() const { return useControlNameAsTitle_; }
	/// Returns the value.
	double value() const { return value_; }
	/// Returns whether using the control value as the editor value.
	bool useControlValueAsValue() const { return useControlValueAsValue_; }
	/// Returns the value format.
	QChar format() const { return format_; }
	/// Returns the value precision.
	int precision() const { return precision_; }
	/// Returns whether using the control precision as the editor precision.
	bool useControlPrecisionAsPrecision() const { return useControlPrecisionAsPrecision_; }
	/// Returns the minimum value.
	double minimumValue() const { return minimumValue_; }
	/// Returns whether using the control minimum as the editor minimum.
	bool useControlMinimumAsMinimum() const { return useControlMinimumAsMinimum_; }
	/// Returns the maximum value.
	double maximumValue() const { return maximumValue_; }
	/// Returns whether using the control maximum as the editor maximum.
	bool useControlMaximumAsMaximum() const { return useControlMaximumAsMaximum_; }
	/// Returns the value options.
	QStringList values() const { return values_; }
	/// Returns whether using the control values as the editor values.
	bool useControlValuesAsValues() const { return useControlValuesAsValues_; }
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
	/// Notifier that the value has changed.
	void valueChanged(double newValue);
	/// Notifier that the flag indicating whether the control's value is used as the editor value has changed.
	void useControlValueAsValueChanged(bool useValue);
	/// Notifier that the value format has changed.
	void formatChanged(const QChar &newFormat);
	/// Notifier that the value precision has changed.
	void precisionChanged(int newPrecision);
	/// Notifier that the flag indicating whether the control's precision is used as the editor precision has changed.
	void useControlPrecisionAsPrecisionChanged(bool usePrecision);
	/// Notifier that the minimum value has changed.
	void minimumValueChanged(double newValue);
	/// Notifier that the flag indicating whether the control's minimum value is used as the editor's minimum has changed.
	void useControlMinimumAsMinimumChanged(bool useMinimum);
	/// Notifier that the maximum value has changed.
	void maximumValueChanged(double newValue);
	/// Notifier that the flag indicating whether the control's maximum value is used as the editor's maximum has changed.
	void useControlMaximumAsMaximumChanged(bool useMaximum);
	/// Notifier that the values have changed.
	void valuesChanged(const QStringList &newValues);
	/// Notifier that the flag indicating whether the control's values are used as the editor's values has changed.
	void useControlValuesAsValuesChanged(bool useValues);
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
	/// Sets the value.
	void setValue(double newValue);
	/// Sets whether the control's value is used as the editor value.
	void setUseControlValueAsValue(bool useValue);
	/// Sets the value format.
	void setFormat(const QChar &newFormat);
	/// Sets the value precision.
	void setPrecision(int newPrecision);
	/// Sets whether the control's precision is used as the editor's precision.
	void setUseControlPrecisionAsPrecision(bool usePrecision);
	/// Sets the minimum value.
	void setMinimumValue(double minimumValue);
	/// Sets whether the control's minimum is used as the editor's minimum.
	void setUseControlMinimumAsMinimum(bool useMinimum);
	/// Sets the maximum value.
	void setMaximumValue(double maximumValue);
	/// Sets whether the control's maximum is used as the editor's maximum.
	void setUseControlMaximumAsMaximum(bool useMaximum);
	/// Sets the values.
	void setValues(const QStringList &newValues);
	/// Sets whether the control's values are used as the editor's values.
	void setUseControlValuesAsValues(bool useValues);
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

	/// Sets the view mode to Display, returns true if successful.
	bool setViewModeToDisplay();
	/// Sets the view mode to Edit, returns true if successful.
	bool setViewModeToEdit();

	/// Sets the title text.
	void setTitleText(const QString &newText);
	/// Sets the units text.
	void setUnitsText(const QString &newText);

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

	/// Returns a new double value. Creates and displays an input dialog to collect user input.
	AMNumber getDoubleValue() const;
	/// Returns a new enum value. Creates and displays an input dialog to collect user input.
	AMNumber getEnumValue() const;
	/// Returns a new calibrated value. Creates and displays an input dialog to collect user input.
	AMNumber getCalibratedDoubleValue() const;

	/// Handles displaying context menu options when requested.
	void onContextMenuRequested(const QPoint &clickPosition);

protected:
	/// Returns true if the view mode can currently be set to Display, false otherwise. Should always return true, as the Display option is always available.
	bool canSetViewModeToDisplay() const;
	/// Returns true if the view mode can currently be set to Edit, false otherwise.
	bool canSetViewModeToEdit() const;

	/// Sets up the spinbox, usually only performed once when the spinbox becomes visible.
	void setupSpinBox();
	/// Sets up the combobox, usually only performed once when the combobox becomes visible.
	void setupComboBox();

	/// Generates and returns text for the value label.
	QString generateValueLabelText() const;
	/// Generates and returns the value label visible status.
	bool generateValueLabelVisibleStatus() const;

	/// Handles the mouse release event--changes the view mode to Edit.
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	/// The control being edited.
	AMControl* control_;

	/// The title text.
	QString title_;
	/// Flag indicating whether to use the control's name as the title text.
	bool useControlNameAsTitle_;
	/// The value.
	double value_;
	/// Flag indicating whether to use the control's value as the editor's value.
	bool useControlValueAsValue_;
	/// The value format.
	QChar format_;
	/// The value precision.
	int precision_;
	/// Flag indicating whether to use the control's precision as the editor's precision.
	bool useControlPrecisionAsPrecision_;
	/// The minimum value.
	double minimumValue_;
	/// Flag indicating whether to use the control's minimum value as the editor's minimum.
	bool useControlMinimumAsMinimum_;
	/// The maximum value.
	double maximumValue_;
	/// Flag indicating whether to use the control's maximum value as the editor's maximum.
	bool useControlMaximumAsMaximum_;
	/// The value options list.
	QStringList values_;
	/// Flag indicating whether to use the control's values as the editor's values.
	bool useControlValuesAsValues_;
	/// The units text.
	QString units_;
	/// Flag indicating whether to use the control's units as the units text.
	bool useControlUnitsAsUnits_;
	/// The read-only status.
	bool readOnly_;

	/// The move action.
	QAction *moveAction_;
	/// The stop action.
	QAction *stopAction_;
	/// The calibrate action.
	QAction *calibrateAction_;

	/// The value label.
	QLabel *valueLabel_;
};

#endif // BIOXASCONTROLEDITOR_H
