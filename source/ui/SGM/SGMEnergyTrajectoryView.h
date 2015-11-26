#ifndef SGMENERGYTRAJECTORYVIEW_H
#define SGMENERGYTRAJECTORYVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>

class SGMEnergyControlSet;
/*!
 * A ui class used to control a continuous coordinated energy trajectory move on
 * the SGM beamline.
 */
class SGMEnergyTrajectoryView : public QWidget
{
	Q_OBJECT
public:
	/*!
	 * Creates an instance of an energy trajectory view which will perform a move
	 * using the provided energy control set.
	 * \param controlSet ~ The control set to which the move instructions are to
	 * be sent.
	 */
	explicit SGMEnergyTrajectoryView(SGMEnergyControlSet* controlSet,
	                                 QWidget *parent = 0);

	/*!
	 * Virtual destructor
	 */
	virtual ~SGMEnergyTrajectoryView() {}

signals:

public slots:
protected slots:
	/*!
	 * Slot which handles the start button being clicked. Sets the start, end and
	 * time PVs the respective spinbox values, then pushes a 1 on the start PV.
	 */
	void onStartPushButtonClicked();

	/*!
	 * Slot which handles the controls from the energy control set being connected.
	 * Makes calls to setupData() and setupConnections()
	 */
	void onControlsConnected(bool isConnected);
protected:

	/// Initializes the ui components, and performs layout.
	void setupUi();

	/// Sets up the data of the spinboxes. Sets min/max/units.
	void setupData();

	/// Sets up connections from the widget(s)
	void setupConnections();


	SGMEnergyControlSet* controlSet_;

	QDoubleSpinBox* targetSpinBox_;
	QDoubleSpinBox* timeSpinBox_;
	QPushButton* startButton_;

	bool connectedOnce_;
};

#endif // SGMENERGYTRAJECTORYVIEW_H
