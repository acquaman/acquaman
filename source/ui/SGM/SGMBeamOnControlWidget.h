#ifndef SGMBEAMONCONTROLWIDGET_H
#define SGMBEAMONCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
class AMControl;
/*!
  * A widget class which displays the current beam status, as well as provides buttons for turning the beam on and off.
  */
class SGMBeamOnControlWidget : public QWidget
{
	Q_OBJECT
public:

	/*!
	  * Creates a Beam On Control Widget which will control the beam on actions represented by the 3 provided
	  * controls.
	  */
	explicit SGMBeamOnControlWidget(AMControl* beamStatusControl,
				  AMControl* beamOnControl,
				  AMControl* beamOffControl,
				  QWidget *parent = 0);

signals:

public slots:
protected slots:
	/// Handles the connection state of the three required controls being changed.
	void onRequiredControlsConnectionChanged(bool isConnected);

	/// Handles the beam on button being pushed
	void onBeamOnButtonPushed();

	/// Handles the beam off button being pushed
	void onBeamOffButtonPushed();

	/// Handles the beam status PV changing
	void onBeamStatusPVChanged();
protected:
	/// Helper method which initializes the child widgets, and perform the layout
	void setupUi();

	/// Helper method which sets up the initial data.
	void setupData();

	/// Helper method which performs the connections required between the controls and the widgets.
	void setupConnections();

	QLabel* beamStatusLabel_;
	QPushButton* beamOnButton_;
	QPushButton* beamOffButton_;

	AMControl* beamStatusControl_;
	AMControl* beamOnControl_;
	AMControl* beamOffControl_;

	bool connectedOnce_;
};

#endif // SGMBEAMONCONTROLWIDGET_H
