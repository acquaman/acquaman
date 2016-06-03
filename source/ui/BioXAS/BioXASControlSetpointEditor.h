#ifndef BIOXASCONTROLSETPOINTEDITOR_H
#define BIOXASCONTROLSETPOINTEDITOR_H

#include "ui/CLS/CLSValueSetpointEditor.h"

class AMControl;

class BioXASControlSetpointEditor : public CLSValueSetpointEditor
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASControlSetpointEditor(AMControl *control, bool showFeedback = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlSetpointEditor();

	/// Returns the control being edited.
	AMControl* control() const { return control_; }

	/// Returns whether using the control value as the editor value.
	bool useControlValueSetpointAsValue() const { return useControlValueSetpointAsValue_; }
	/// Returns whether using the control value feedback as the editor value feedback.
	bool useControlValueFeedbackAsValueFeedback() const { return useControlValueAsValueFeedback_; }
	/// Returns whether using the control precision as the editor precision.
	bool useControlPrecisionAsPrecision() const { return useControlPrecisionAsPrecision_; }
	/// Returns whether using the control minimum as the editor minimum.
	bool useControlMinimumAsMinimum() const { return useControlMinimumAsMinimum_; }
	/// Returns whether using the control maximum as the editor maximum.
	bool useControlMaximumAsMaximum() const { return useControlMaximumAsMaximum_; }
	/// Returns whether using the control move values as the editor move values.
	bool useControlMoveValuesAsValues() const { return useControlMoveValuesAsValues_; }
	/// Returns whether using the control units as the editor units.
	bool useControlUnitsAsUnits() const { return useControlUnitsAsUnits_; }
	/// Returns whether using control's enum status as the editor's input type.
	bool useControlEnumStatusAsEditorInputType() const { return useControlEnumStatusAsEditorInputType_; }

signals:
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the flag indicating whether the control's value is used as the editor value has changed.
	void useControlValueSetpointAsValueChanged(bool useValue);
	/// Notifier that the flag indicating whether the control's value feedback is used as the editor value feedback has changed.
	void useControlValueAsValueFeedbackChanged(bool useValue);
	/// Notifier that the flag indicating whether the control's precision is used as the editor precision has changed.
	void useControlPrecisionAsPrecisionChanged(bool usePrecision);
	/// Notifier that the flag indicating whether the control's minimum value is used as the editor's minimum has changed.
	void useControlMinimumAsMinimumChanged(bool useMinimum);
	/// Notifier that the flag indicating whether the control's maximum value is used as the editor's maximum has changed.
	void useControlMaximumAsMaximumChanged(bool useMaximum);
	/// Notifier that the flag indicating whether the control's move values are used as the editor's move values has changed.
	void useControlMoveValuesAsValuesChanged(bool useValues);
	/// Notifier that the flag indicating whether the control's units are used as the editor's units has changed.
	void useControlUnitsAsUnitsChanged(bool useUnits);
	/// Notifier that the flag indicating whether the control's enum status is used as the editor's input type has changed.
	void useControlEnumStatusAsEditorInputTypeChanged(bool useStatus);

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
	void setUseControlValueAsValueFeedback(bool useValue);
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
	/// Sets whether the control's move values are used as the editor's move values.
	void setUseControlMoveValuesAsValues(bool useValues);
	/// Sets the units text.
	virtual void setUnits(const QString &newUnits);
	/// Sets whether the control's units are used for the units text.
	void setUseControlUnitsAsUnits(bool useUnits);
	/// Sets the input type.
	virtual void setInputType(CLSValueSetpointEditor::InputType newType);
	/// Sets whether the control's enum status is used for the editor's input type.
	void setUseControlEnumStatusAsInputType(bool useStatus);

protected slots:
	/// Initializes the editor value to be the control's current value.
	void initializeValue();

	/// Updates the value.
	void updateValue();
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
	/// Updates the units.
	void updateUnits();
	/// Updates the input type.
	void updateInputType();

protected:
	/// The control being edited.
	AMControl *control_;

	/// Flag indicating whether to use the control's value setpoint as the editor's value.
	bool useControlValueSetpointAsValue_;
	/// Flag indicating whether to use the control's value feedback as the editor's value feedback.
	bool useControlValueAsValueFeedback_;
	/// Flag indicating whether to use the control's precision as the editor's precision.
	bool useControlPrecisionAsPrecision_;
	/// Flag indicating whether to use the control's minimum value as the editor's minimum.
	bool useControlMinimumAsMinimum_;
	/// Flag indicating whether to use the control's maximum value as the editor's maximum.
	bool useControlMaximumAsMaximum_;
	/// Flag indicating whether to use the control's move values as the editor's move values.
	bool useControlMoveValuesAsValues_;
	/// Flag indicating whether to use the control's units as the units text.
	bool useControlUnitsAsUnits_;
	/// Flag indicating whether to use the control's enum status as the editor's input type.
	bool useControlEnumStatusAsEditorInputType_;
};

#endif // BIOXASCONTROLSETPOINTEDITOR_H
