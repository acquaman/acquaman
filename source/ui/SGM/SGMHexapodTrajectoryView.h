#ifndef SGMHEXAPODTRAJECTORYVIEW_H
#define SGMHEXAPODTRAJECTORYVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

class SGMHexapod;
/*!
  * A widget view class which provides a ui for performing trajectory moves on
  * an SGMHexapod
  */
class SGMHexapodTrajectoryView : public QWidget
{
    Q_OBJECT
public:
	/*!
	  * Creates a view which will control trajectory moves on the provided hexapod
	  * \param hexapod ~ The hexapod on which trajectory moves will be performed.
	  */
	explicit SGMHexapodTrajectoryView(SGMHexapod* hexapod,
									  QWidget *parent = 0);

	/*!
	  * Virtual destructor for a hexapod trajectory view
	  */
	virtual ~SGMHexapodTrajectoryView() {}
signals:

public slots:
	/*!
	  * Handles signals indicating that the move button has been clicked. Sets
	  * the hexapod's trajectory move PV to 1 (starting a move).
	  */
	void onMoveButtonClicked();

	/*!
	  * Handles signals indicating that the reset button has been clicked. Sets
	  * the hexapod's trajectory reset PV to 1 (setting all the setpoint back
	  * to the last target position)
	  */
	void onResetButtonClicked();

	/*!
	  * Handles signals indicating that editing has finished on the x spinbox.
	  * Sets the x trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onXSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals indicating that editing has finished on the y spinbox.
	  * Sets the y trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onYSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals indicating that editing has finished on the z spinbox.
	  * Sets the z trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onZSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals indicating that editing has finished on the u spinbox.
	  * Sets the u trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onUSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals indicating that editing has finished on the v spinbox.
	  * Sets the v trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onVSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals indicating that editing has finished on the w spinbox.
	  * Sets the w trajectory pv to the spin box's current value and sets the
	  * background colour of the spinbox to indicate editing if the set value is
	  * different to the pv value.
	  */
	void onWSetpointSpinboxEditingFinished();

	/*!
	  * Handles signals from the x trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onXSetpointPVValueChanged();

	/*!
	  * Handles signals from the y trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onYSetpointPVValueChanged();

	/*!
	  * Handles signals from the z trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onZSetpointPVValueChanged();

	/*!
	  * Handles signals from the u trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onUSetpointPVValueChanged();

	/*!
	  * Handles signals from the v trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onVSetpointPVValueChanged();

	/*!
	  * Handles signals from the w trajectory PV indicating that its value has
	  * changed. Updates the value in the spinbox and sets its background colour
	  * to white if the pv value is different from the spinbox value.
	  */
	void onWSetpointPVValueChanged();
protected:
	/*!
	  * Initializes ui components and performs layout
	  */
	void setupUi();

	/*!
	  * Initializes the values in the controls based on the hexapod's data.
	  */
	void setupData();

	/*!
	  * Connects the required signals and slots.
	  */
	void setupConnections();

	SGMHexapod* hexapod_;
	QLabel* xSetpointLabel_;
	QLabel* ySetpointLabel_;
	QLabel* zSetpointLabel_;
	QLabel* uSetpointLabel_;
	QLabel* vSetpointLabel_;
	QLabel* wSetpointLabel_;

	QDoubleSpinBox* xSetpointSpinbox_;
	QDoubleSpinBox* ySetpointSpinbox_;
	QDoubleSpinBox* zSetpointSpinbox_;
	QDoubleSpinBox* uSetpointSpinbox_;
	QDoubleSpinBox* vSetpointSpinbox_;
	QDoubleSpinBox* wSetpointSpinbox_;

	QPushButton* moveButton_;
	QPushButton* resetButton_;

};

#endif // SGMHEXAPODTRAJECTORYVIEW_H
