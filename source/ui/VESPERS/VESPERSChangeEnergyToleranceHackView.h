#ifndef VESPERSCHANGEENERGYTOLERANCEHACKVIEW_H
#define VESPERSCHANGEENERGYTOLERANCEHACKVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>

class VESPERSChangeEnergyToleranceHackView : public QWidget
{
	Q_OBJECT
public:

	/// Constructor.
	explicit VESPERSChangeEnergyToleranceHackView(QWidget *parent = 0);


signals:

public slots:

	/// Slot that sets the new tolerance.
	void setTolerance();
	/// Slot that shows the current feedback value.
	void updateFeedback();

protected:

	/// The current tolerance.
	QLabel *feedback_;
	/// The setpoint box.
	QDoubleSpinBox *setpoint_;
};

#endif // VESPERSCHANGEENERGYTOLERANCEHACKVIEW_H
