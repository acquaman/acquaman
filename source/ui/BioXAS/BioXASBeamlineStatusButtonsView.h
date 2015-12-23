#ifndef BIOXASBEAMLINESTATUSBUTTONSVIEW_H
#define BIOXASBEAMLINESTATUSBUTTONSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

class AMExtendedControlEditor;
class BioXASShuttersButton;
class BioXASValvesButton;

class BioXASBeamlineStatusButtonsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASBeamlineStatusButtonsView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineStatusButtonsView();

signals:

public slots:

protected:
	/// The front-end shutters button.
	BioXASShuttersButton *frontEndShuttersButton_;
	/// The master valves button.
	BioXASValvesButton *valvesButton_;

	/// The editors box.
	QGroupBox *editorsBox_;
	/// The front-end shutters editor.
	AMExtendedControlEditor *frontEndShuttersEditor_;
	/// The valves editor.
	AMExtendedControlEditor *valvesEditor_;
};

#endif // BIOXASBEAMLINESTATUSBUTTONSVIEW_H
