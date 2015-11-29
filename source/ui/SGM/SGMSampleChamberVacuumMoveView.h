#ifndef SGMSAMPLECHAMBERVACUUMMOVEVIEW_H
#define SGMSAMPLECHAMBERVACUUMMOVEVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;
class SGMVATValveView;
class SGMSampleChamberVacuumMoveControl;

class SGMSampleChamberVacuumMoveView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumMoveView(SGMSampleChamberVacuumMoveControl *move, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveView();

	/// Returns the move control being viewed.
	SGMSampleChamberVacuumMoveControl* move() const { return move_; }

signals:
	/// Notifier that the move control being viewed has changed.
	void moveChanged(SGMSampleChamberVacuumMoveControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the move control being viewed.
	void setMove(SGMSampleChamberVacuumMoveControl *newControl);

protected slots:
	/// Clears the view.
	void clear();

protected:
	/// The move control being viewed.
	SGMSampleChamberVacuumMoveControl *move_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEVIEW_H
