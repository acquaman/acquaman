#ifndef BIOXASCONTROLSETPOINTEDITOR_H
#define BIOXASCONTROLSETPOINTEDITOR_H

#include "ui/BioXAS/BioXASValueSetpointEditor.h"

class AMControl;

class BioXASControlSetpointEditor : public BioXASValueSetpointEditor
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASControlSetpointEditor(AMControl *control, BioXASValueSetpointEditor::InputType type = BioXASValueSetpointEditor::TypeDouble, bool showFeedback = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlSetpointEditor();

	/// Returns the control being edited.
	AMControl* control() const { return control_; }

	/// Returns whether using the control value as the editor value.
	bool useControlValueSetpointAsValue() const { return useControlValueSetpointAsValue_; }
	/// Returns whether using the control value feedback as the editor value feedback.
	bool useControlValueFeedbackAsValueFeedback() const { return useControlValueFeedbackAsValueFeedback_; }
	/// Returns whether using the control precision as the editor precision.
	bool useControlPrecisionAsPrecision() const { return useControlPrecisionAsPrecision_; }
	/// Returns whether using the control minimum as the editor minimum.
	bool useControlMinimumAsMinimum() const { return useControlMinimumAsMinimum_; }
	/// Returns whether using the control maximum as the editor maximum.
	bool useControlMaximumAsMaximum() const { return useControlMaximumAsMaximum_; }
	/// Returns whether using the control values as the editor values.
	bool useControlValuesAsValues() const { return useControlValuesAsValues_; }
	/// Returns whether using the control move values as the editor move values.
	bool useControlMoveValuesAsMoveValues() const { return useControlMoveValuesAsMoveValues_; }
	/// Returns whether using the control units as the editor units.
	bool useControlUnitsAsUnits() const { return useControlUnitsAsUnits_; }

signals:
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the flag indicating whether the control's value is used as the editor value has changed.
	void useControlValueSetpointAsValueChanged(bool useValue);
	/// Notifier that the flag indicating whether the control's value feedback is used as the editor value feedback has changed.
	void useControlValueFeedbackAsValueFeedbackChanged(bool useValue);
	/// Notifier that the flag indicating whether the control's precision is used as the editor precision has changed.
	void useControlPrecisionAsPrecisionChanged(bool usePrecision);
	/// Notifier that the flag indicating whether the control's minimum value is used as the editor's minimum has changed.
	void useControlMinimumAsMinimumChanged(bool useMinimum);
	/// Notifier that the flag indicating whether the control's maximum value is used as the editor's maximum has changed.
	void useControlMaximumAsMaximumChanged(bool useMaximum);
	/// Notifier that the flag indicating whether the control's values are used as the editor's values has changed.
	void useControlValuesAsValuesChanged(bool useValues);
	/// Notifier that the flag indicating whether the control's move values are used as the editor's move values has changed.
	void useControlMoveValuesAsMoveValuesChanged(bool useValues);
	/// Notifier that the flag indicating whether the control's units are used as the editor's units has changed.
	void useControlUnitsAsUnitsChanged(bool useUnits);

public slots:
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

	/// Sets the value.
	virtual void setValue(double newValue);
	/// Sets whether the control's value is used as the editor value.
	void setUseControlValueSetpointAsValue(bool useValue);
	/// Sets the value.
	virtual void setValueFeedback(double newValue);
	/// Sets whether the control's value is used as the editor value.
	void setUseControlValueFeedbackAsValueFeedback(bool useValue);
	/// Sets the value precision.
	virtual void setPrecision(int newPrecision);
	/// Sets whether the control's precision is used as the editor's precision.
	void setUseControlPrecisionAsPrecision(bool usePrecision);
	/// Sets the minimum value.
	virtual void setMinimumValue(double minimumValue);
	/// Sets whether the control's minimum is used as the editor's minimum.
	void setUseControlMinimumAsMinimum(bool useMinimum);
	/// Sets the maximum value.
	virtual void setMaximumValue(double maximumValue);
	/// Sets whether the control's maximum is used as the editor's maximum.
	void setUseControlMaximumAsMaximum(bool useMaximum);
	/// Sets the values.
	virtual void setValues(const QStringList &newValues);
	/// Sets whether the control's values are used as the editor's values.
	void setUseControlValuesAsValues(bool useValues);
	/// Sets the move values.
	virtual void setMoveValues(const QStringList &newValues);
	/// Sets whether the control's move values are used as the editor's move values.
	void setUseControlMoveValuesAsMoveValues(bool useValues);
	/// Sets the units text.
	virtual void setUnits(const QString &newUnits);
	/// Sets whether the control's units are used for the units text.
	void setUseControlUnitsAsUnits(bool useUnits);

protected slots:
	/// Updates the value setpoint.
	void updateValueSetpoint();
	/// Updates the value feedback.
	void updateValueFeedback();
	/// Updates the precision.
	void updatePrecision();
	/// Updates the minimum value.
	void updateMinimumValue();
	/// Updates the maximum value.
	void updateMaximumValue();
	/// Updates the values.
	void updateValues();
	/// Updates the move values.
	void updateMoveValues();
	/// Updates the units.
	void updateUnits();

protected:
	/// Generates and returns value label text for the editor's current information.
	virtual QString generateValueText() const;
	/// Generates and returns value label units text for the editor's current information.
	virtual QString generateUnitsText() const;

protected:
	/// The control being edited.
	AMControl *control_;

	/// Flag indicating whether to use the control's value setpoint as the editor's value.
	bool useControlValueSetpointAsValue_;
	/// Flag indicating whether to use the control's value feedback as the editor's value feedback.
	bool useControlValueFeedbackAsValueFeedback_;
	/// Flag indicating whether to use the control's precision as the editor's precision.
	bool useControlPrecisionAsPrecision_;
	/// Flag indicating whether to use the control's minimum value as the editor's minimum.
	bool useControlMinimumAsMinimum_;
	/// Flag indicating whether to use the control's maximum value as the editor's maximum.
	bool useControlMaximumAsMaximum_;
	/// Flag indicating whether to use the control's values as the editor's values.
	bool useControlValuesAsValues_;
	/// Flag indicating whether to use the control's move values as the editor's move values.
	bool useControlMoveValuesAsMoveValues_;
	/// Flag indicating whether to use the control's units as the units text.
	bool useControlUnitsAsUnits_;
};

#endif // BIOXASCONTROLSETPOINTEDITOR_H
