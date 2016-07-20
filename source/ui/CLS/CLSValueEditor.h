#ifndef CLSVALUEEDITOR_H
#define CLSVALUEEDITOR_H

#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QApplication>
#include <QMouseEvent>

#include "dataman/AMNumber.h"
#include "ui/CLS/CLSValueProgressLabel.h"

class CLSValueEditor : public QGroupBox
{
	Q_OBJECT

	Q_PROPERTY(EditStatus editStatus READ editStatus WRITE setEditStatus NOTIFY editStatusChanged)

	Q_ENUMS(EditStatus)

public:
	/// Enumeration of the editing status.
	enum EditStatus { NotEditing = 0, Editing };

	/// Constructor.
	explicit CLSValueEditor(QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSValueEditor();

	/// Returns the editing status.
	CLSValueEditor::EditStatus editStatus() const { return editStatus_; }

	/// Returns the title text.
	QString title() const { return title_; }
	/// Returns the value.
	AMNumber value() const { return value_; }
	/// Returns the value format.
	QChar format() const { return format_; }
	/// Returns the value precision.
	int precision() const { return precision_; }
	/// Returns the minimum value.
	double minimumValue() const { return minimumValue_; }
	/// Returns the maximum value.
	double maximumValue() const { return maximumValue_; }
	/// Returns the value options.
	QStringList values() const { return values_; }
	/// Returns the move value options.
	QStringList moveValues() const { return moveValues_; }
	/// Returns the units.
	QString units() const { return units_; }
	/// Returns the current read-only status of the editor.
	bool readOnly() const { return readOnly_; }

signals:
	/// Notifier that the edit status has changed.
	void editStatusChanged(CLSValueEditor::EditStatus newStatus);

	/// Notifier that the title text has changed.
	void titleChanged(const QString &newText);
	/// Notifier that the value has changed.
	void valueChanged(const AMNumber &newValue);
	/// Notifier that the value format has changed.
	void formatChanged(const QChar &newFormat);
	/// Notifier that the value precision has changed.
	void precisionChanged(int newPrecision);
	/// Notifier that the minimum value has changed.
	void minimumValueChanged(double newValue);
	/// Notifier that the maximum value has changed.
	void maximumValueChanged(double newValue);
	/// Notifier that the values have changed.
	void valuesChanged(const QStringList &newValues);
	/// Notifier that the move values have changed.
	void moveValuesChanged(const QStringList &newValues);
	/// Notifier that the units text has changed.
	void unitsChanged(const QString &newUnits);
	/// Notifier that the read-only status of the editor has changed.
	void readOnlyChanged(bool readOnly);
	/// Notifier that the progress value minimum has changed.
	void progressValueMinimumChanged(double newValue);
	/// Notifier that the progress value maximum has changed.
	void progressValueMaximumChanged(double newValue);
	/// Notifier tha the progress value has changed.
	void progressValueChanged(double newValue);
	/// Notifier that the flag indicating whether the progress is displayed has changed.
	void displayProgressChanged(bool showProgress);

public slots:
	/// Refreshes the editor.
	virtual void refresh();

	/// Sets the editor title text.
	virtual void setTitle(const QString &newText);
	/// Sets the value.
	virtual void setValue(const AMNumber &newValue);
	/// Sets the value format.
	virtual void setFormat(const QChar &newFormat);
	/// Sets the value precision.
	virtual void setPrecision(int newPrecision);
	/// Sets the minimum value.
	virtual void setMinimumValue(double minimumValue);
	/// Sets the maximum value.
	virtual void setMaximumValue(double maximumValue);
	/// Sets the values.
	virtual void setValues(const QStringList &newValues);
	/// Sets the move values.
	virtual void setMoveValues(const QStringList &newValues);
	/// Sets the units text.
	virtual void setUnits(const QString &newUnits);
	/// Sets whether the editor is read-only.
	virtual void setReadOnly(bool readOnly);

	/// Sets the progress value minimum.
	virtual void setProgressValueMinimum(double newValue);
	/// Sets the progress value maximum.
	virtual void setProgressValueMaximum(double newValue);
	/// Sets the progress value.
	virtual void setProgressValue(double newValue);
	/// Sets the flag for whether the progress is displayed.
	virtual void setDisplayProgress(bool showProgress);

protected slots:
	/// Sets the editing status.
	void setEditStatus(CLSValueEditor::EditStatus newStatus);

	/// Updates the title.
	virtual void updateTitle();
	/// Updates the value label.
	virtual void updateValueLabel();
	/// Updates the edit action.
	virtual void updateEditAction();

	/// Initiates the process of editing the value and updating the relevant states.
	void edit();
	/// Initiates the process of editing the value.
	virtual void editImplementation();
	/// Returns a new value. Creates and displays an input dialog to collect user input. Subclasses can reimplement to define unique behavior.
	virtual AMNumber getValue();
	/// Returns a new double value. Creates and displays an input dialog to collect user input in the form of a double.
	virtual AMNumber getDoubleValue();
	/// Returns a new enum value. Creates and displays an input dialog to collect user input in the form of an enum/int.
	virtual AMNumber getEnumValue();

	/// Handles displaying context menu options when requested.
	virtual void onContextMenuRequested(const QPoint &clickPosition);

	/// Handles initiating a value edit, when the edit action is triggered.
	virtual void onEditActionTriggered();

protected:
	/// Returns true if the given QChar corresponds to a valid format, false otherwise.
	virtual bool validFormat(const QChar &format) const;
	/// Returns true if the given integer corresponds to a valid precision, false otherwise.
	virtual bool validPrecision(int precision) const;

	/// Generates and returns value label text for the editor's current information.
	virtual QString generateValueText() const;
	/// Generates and returns value label units text for the editor's current information.
	virtual QString generateUnitsText() const;

	/// Handles the mouse release event, triggers the edit action.
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	/// The editing status.
	EditStatus editStatus_;

	/// The title text.
	QString title_;
	/// The value.
	AMNumber value_;
	/// The value format.
	QChar format_;
	/// The value precision.
	int precision_;
	/// The minimum value.
	double minimumValue_;
	/// The maximum value.
	double maximumValue_;
	/// The value options list. This list should include all move enums and read-only enums (in that order).
	QStringList values_;
	/// The move value options list. This list should include all move enums.
	QStringList moveValues_;
	/// The units text.
	QString units_;
	/// The read-only status.
	bool readOnly_;
	/// The progress value minimum.
	double progressValueMinimum_;
	/// The progress value maximum.
	double progressValueMaximum_;
	/// The progress value.
	double progressValue_;
	/// The flag indicating whether progress is displayed.
	bool displayProgress_;

	/// The edit action.
	QAction *editAction_;

	/// the content layout to contain the UI components
	QVBoxLayout *contentLayout_;
	/// The value label.
	CLSValueProgressLabel *valueLabel_;
};

#endif // CLSVALUEEDITOR_H
