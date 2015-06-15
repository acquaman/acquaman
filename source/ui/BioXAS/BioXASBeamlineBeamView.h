#ifndef BIOXASBEAMLINEBEAMVIEW_H
#define BIOXASBEAMLINEBEAMVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QPixmap>

class BioXASBeamline;

class BioXASBeamlineBeamView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineBeamView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineBeamView();

protected slots:
	/// Handles updating the view to reflect the current beam status.
	void updateBeamStatus();

	/// Sets the view to show 'beam off'.
	void showBeamOff();
	/// Sets the view to show 'beam on'.
	void showBeamOn();
	/// Sets the view to show 'between'.
	void showBeamBetween();

protected:
	/// The beam status label.
	QLabel *beamStatus_;
	/// The beam on button.
	QPushButton *beamOnButton_;
	/// The beam off button.
	QPushButton *beamOffButton_;
};

#endif // BIOXASBEAMLINEBEAMVIEW_H
