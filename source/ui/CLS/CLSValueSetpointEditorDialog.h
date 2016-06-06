#ifndef CLSVALUESETPOINTEDITORDIALOG_H
#define CLSVALUESETPOINTEDITORDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLayout>

class CLSValueSetpointEditor;

class CLSValueSetpointEditorDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.
	CLSValueSetpointEditorDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	/// Destructor.
	virtual ~CLSValueSetpointEditorDialog();

	/// Returns the current value.
	double value() const;
	/// Returns true if the minimum value has been set.
	bool minimumSet() const;
	/// Returns the minimum value.
	double minimum() const;
	/// Returns true if the maximum value has been set.
	bool maximumSet() const;
	/// Returns the maximum value.
	double maximum() const;

public slots:
	/// Sets the current value.
	void setValue(double newValue);
	/// Set the value options to choose from, for enum values.
	void setValues(const QStringList &newValues);
	/// Sets the minimum value.
	void setMinimumValue(double newMin);
	/// Sets the maximum value.
	void setMaximumValue(double newMax);

protected slots:
	/// Updates the buttons.
	void updateButtons();

protected:
	/// The setpoint editor.
	CLSValueSetpointEditor *setpointEditor_;
	/// The dialog button box.
	QDialogButtonBox *buttonBox_;
};

#endif // CLSVALUESETPOINTEDITORDIALOG_H
