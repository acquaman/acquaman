#ifndef BIOXASVALUEEDITOR_H
#define BIOXASVALUEEDITOR_H

#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QApplication>
#include <QMouseEvent>

#include "dataman/AMNumber.h"

class BioXASValueEditor : public QGroupBox
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValueEditor(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValueEditor();

	/// Returns the title text.
	QString title() const { return title_; }
	/// Returns the value.
	double value() const { return double(value_); }
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
	/// Returns the units.
	QString units() const { return units_; }
	/// Returns the current read-only status of the editor.
	bool readOnly() const { return readOnly_; }

signals:
	/// Notifier that the title text has changed.
	void titleChanged(const QString &newText);
	/// Notifier that the value has changed.
	void valueChanged(double newValue);
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
	/// Notifier that the units text has changed.
	void unitsChanged(const QString &newUnits);
	/// Notifier that the read-only status of the editor has changed.
	void readOnlyChanged(bool readOnly);

public slots:
	/// Refreshes the editor.
	virtual void refresh();

	/// Updates the title.
	virtual void updateTitle();
	/// Updates the value label.
	virtual void updateValueLabel();

	/// Sets the editor title text.
	virtual void setTitle(const QString &newText);
	/// Sets the value.
	virtual void setValue(double newValue);
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
	/// Sets the units text.
	virtual void setUnits(const QString &newUnits);
	/// Sets whether the editor is read-only.
	virtual void setReadOnly(bool readOnly);

protected slots:
	/// Returns a new double value. Creates and displays an input dialog to collect user input.
	virtual AMNumber getDoubleValue() const;
	/// Returns a new enum value. Creates and displays an input dialog to collect user input.
	virtual AMNumber getEnumValue() const;

	/// Handles displaying context menu options when requested.
	virtual void onContextMenuRequested(const QPoint &clickPosition);

protected:
	/// Returns true if the given QChar corresponds to a valid format, false otherwise.
	virtual bool validFormat(const QChar &format) const;
	/// Returns true if the given integer corresponds to a valid precision, false otherwise.
	virtual bool validPrecision(int precision) const;

	/// Generates and returns value label text for the editor's current information.
	virtual QString generateValueText() const;

	/// Handles the mouse release event, triggers the edit action.
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	/// The title text.
	QString title_;
	/// The value.
	AMNumber value_;
	/// The value text.
	QString valueText_;
	/// The value format.
	QChar format_;
	/// The value precision.
	int precision_;
	/// The minimum value.
	double minimumValue_;
	/// The maximum value.
	double maximumValue_;
	/// The value options list.
	QStringList values_;
	/// The units text.
	QString units_;
	/// The read-only status.
	bool readOnly_;

	/// The value label.
	QLabel *valueLabel_;
};

#endif // BIOXASVALUEEDITOR_H
