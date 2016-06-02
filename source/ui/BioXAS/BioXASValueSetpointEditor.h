#ifndef BIOXASVALUESETPOINTEDITOR_H
#define BIOXASVALUESETPOINTEDITOR_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

class BioXASValueSetpointEditor : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(InputStatus inputStatus READ inputStatus WRITE setInputStatus NOTIFY inputStatusChanged)
	Q_ENUMS(InputStatus)

	Q_PROPERTY(InputType inputType READ inputType WRITE setInputType NOTIFY inputTypeChanged)
	Q_ENUMS(InputType)

public:
	/// Enumeration of the input status.
	enum InputStatus { StatusNone = 0, StatusBad = 1 };
	/// Enumeration of the input type.
	enum InputType { TypeDouble = 0, TypeEnum = 1 };

	/// Constructor.
	explicit BioXASValueSetpointEditor(InputType type = TypeDouble, bool showFeedback = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValueSetpointEditor();

	/// Returns the input status.
	InputStatus inputStatus() const { return status_; }
	/// Returns the input type.
	InputType inputType() const { return type_; }
	/// Returns the current value.
	double value() const;
	/// Returns true if the minimum value has been set.
	bool minimumSet() const { return minimumSet_; }
	/// Returns the minimum value.
	double minimum() const { return minimum_; }
	/// Returns true if the maximum value has been set.
	bool maximumSet() const { return maximumSet_; }
	/// Returns the maximum value.
	double maximum() const { return maximum_; }
	/// Returns the flag for whether the feedback value is displayed.
	bool displayFeedbackValue() const { return displayFeedbackValue_; }
	/// Returns the feedback value.
	double feedbackValue() const { return feedbackValue_; }

	/// Returns true if the minimum value is set and the current value is less than the minimum, indicating a bad input state.
	bool valueLessThanMinimum() const;
	/// Returns true if the maximum value is set and the current value is greater than the maximum, indicating a bad input state.
	bool valueGreaterThanMaximum() const;

signals:
	/// Notifier that the input status has changed.
	void inputStatusChanged(InputStatus newStatus);
	/// Notifier that the input type has changed.
	void inputTypeChanged(InputType newType);
	/// Notifier that the value has changed.
	void valueChanged(double newValue);
	/// Notifier that the values have changed.
	void valuesChanged(const QStringList &newValues);
	/// Notifier that the minimum value has changed.
	void minimumChanged(double newMin);
	/// Notifier that the maximum value has changed.
	void maximumChanged(double newMax);
	/// Notifier that the flag for whether a feedback value is displayed has changed.
	void displayFeedbackValueChanged(bool showFeedback);
	/// Notifier that the feedback value has changed.
	void feedbackValueChanged(double newValue);

public slots:
	/// Sets the input type.
	void setInputType(InputType newType);

	/// Sets the current value.
	void setValue(double newValue);
	/// Set the value options to choose from, for enum values.
	void setValues(const QStringList &newValues);
	/// Sets the minimum value.
	void setMinimum(double newMin);
	/// Sets the maximum value.
	void setMaximum(double newMax);
	/// Sets the flag for whether the feedback value is displayed.
	void setDisplayFeedbackValue(bool showFeedback);
	/// Sets the feedback value.
	void setFeedbackValue(double newValue);

	/// Clears the editor.
	virtual void clear();

protected slots:
	/// Sets the input status.
	void setInputStatus(InputStatus newStatus);

	/// Updates the input status.
	void updateInputStatus();
	/// Updates the tooltip.
	void updateToolTip();
	/// Updates the spinbox and the combobox.
	void updateBoxes();
	/// Updates the feedback label.
	void updateFeedbackLabel();

	/// Handles emitting the appropriate signals when one of the box widget's value changes.
	void onBoxValueChanged();

protected:
	/// Handles painting the widget. Reimplemented for stylesheet to take effect.
	virtual void paintEvent(QPaintEvent *e);

	/// Returns the value, based on the current input type.
	virtual double getValue() const;
	/// Returns the input status, based on the current input type and available min and max values.
	virtual InputStatus getInputStatus() const;
	/// Returns the tooltip text, based on the current input status, value, and available min and max values.
	virtual QString getToolTipText() const;

protected:
	/// The input status.
	InputStatus status_;
	/// The input type.
	InputType type_;

	/// Flag indicating whether a minimum has been set.
	bool minimumSet_;
	/// The minimum value.
	double minimum_;
	/// Flag indicating whether a maximum has been set.
	bool maximumSet_;
	/// The maximum value.
	double maximum_;

	/// Flag indicating whether a feedback value should be displayed.
	bool displayFeedbackValue_;
	/// The display feedback value.
	double feedbackValue_;

	/// The input spinbox.
	QDoubleSpinBox *spinBox_;
	/// The input combobox.
	QComboBox *comboBox_;
	/// The feedback label.
	QLabel *feedbackLabel_;
};

#endif // BIOXASVALUESETPOINTEDITOR_H
