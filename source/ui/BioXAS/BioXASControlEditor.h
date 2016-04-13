#ifndef BIOXASCONTROLEDITOR_H
#define BIOXASCONTROLEDITOR_H

#include "ui/BioXAS/BioXASValueEditor.h"

class AMControl;
class AMControlView;

class BioXASControlEditor : public BioXASValueEditor
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
	/// Returns whether using the control value as the editor value.
	bool useControlValueAsValue() const { return useControlValueAsValue_; }
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
	/// Notifier that the flag indicating whether the control's name is used as the title text has changed.
	void useControlNameAsTitleChanged(bool useName);
	/// Notifier that the flag indicating whether the control's value is used as the editor value has changed.
	void useControlValueAsValueChanged(bool useValue);
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
	/// Notifier that the flag indicating whetherh the control's moving state is used to trigger displaying progress has changed.
	void useControlMovingStateToDisplayProgressChanged(bool useState);

public slots:
	/// Refreshes the editor.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

	/// Sets the editor title text.
	virtual void setTitle(const QString &title);
	/// Sets whether the control's name is used for the title text.
	void setUseControlNameAsTitle(bool useName);
	/// Sets the value.
	virtual void setValue(const AMNumber &newValue);
	/// Sets whether the control's value is used as the editor value.
	void setUseControlValueAsValue(bool useValue);
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
	/// Sets whether the control's moving status is used to trigger displaying progress.
	void setUseControlMovingStateToDisplayProgress(bool useState);

protected slots:
	/// Updates the title text.
	void updateTitleText();
	/// Updates the value.
	void updateValue();
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
	/// Updates the progress value minimum.
	void updateProgressValueMinimum();
	/// Updates the progress value maximum.
	void updateProgressValueMaximum();
	/// Updates the progress value.
	void updateProgressValue();
	/// Updates the flag for whether the progress is displayed.
	void updateDisplayProgress();

	/// Updates the actions.
	virtual void updateActions();
	/// Updates the move action.
	virtual void updateEditAction();
	/// Updates the stop action.
	virtual void updateStopAction();
	/// Updates the calibrate action.
	virtual void updateCalibrateAction();
	/// Updates the properties action.
	virtual void updatePropertiesAction();

	/// Handles initiating a value edit, when the edit action is triggered.
	void onEditActionTriggered();
	/// Handles initiating a stop, when the stop action is triggered.
	void onStopActionTriggered();
	/// Handles initiating a calibration, when the calibrate action is triggered.
	void onCalibrateActionTriggered();
	/// Handles creating and displaying a control view.
	void onPropertiesActionTriggered();

	/// Returns a new calibrated value. Creates and displays an input dialog to collect user input.
	virtual AMNumber getCalibratedDoubleValue();

	/// Handles displaying context menu options when requested.
	virtual void onContextMenuRequested(const QPoint &clickPosition);

protected:
	/// Generates and returns value label text for the editor's current information.
	virtual QString generateValueText() const;
	/// Generates and returns value label units text for the editor's current information.
	virtual QString generateUnitsText() const;

protected:
	/// The control being edited.
	AMControl* control_;

	/// Flag indicating whether to use the control's name as the title text.
	bool useControlNameAsTitle_;
	/// Flag indicating whether to use the control's value as the editor's value.
	bool useControlValueAsValue_;
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

	/// Flag indicating whether to use the control's moving state to indicate progress.
	bool useControlMovingAsProgress_;

	/// The stop action.
	QAction *stopAction_;
	/// The calibrate action.
	QAction *calibrateAction_;
	/// The properties action.
	QAction *propertiesAction_;

	/// The control view.
	AMControlView *controlView_;
};

#endif // BIOXASCONTROLEDITOR_H
