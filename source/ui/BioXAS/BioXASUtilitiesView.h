#ifndef BIOXASUTILITIESVIEW_H
#define BIOXASUTILITIESVIEW_H

#include <QWidget>

class BioXASUtilities;
class BioXASControlEditor;

class BioXASUtilitiesView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASUtilitiesView(BioXASUtilities *utilities, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilitiesView();

	/// Returns the utilities being viewed.
	BioXASUtilities* utilities() const { return utilities_; }

signals:
	/// Notifier that the utilities being viewed have changed.
	void utilitiesChanged(BioXASUtilities *newUtilities);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the utilities being viewed.
	void setUtilities(BioXASUtilities *newUtilities);

protected slots:
	/// Updates the utilities status editor.
	void updateStatusEditor();
	/// Updates the shutters editor.
	void updateShuttersEditor();
	/// Updates the valves editor.
	void updateValvesEditor();
	/// Updates the ion pumps editor.
	void updateIonPumpsEditor();
	/// Updates the flow switches editor.
	void updateFlowSwitchesEditor();
	/// Updates the pressure monitors editor.
	void updatePressureMonitorsEditor();
	/// Updates the temperature monitors editor.
	void updateTemperatureMonitorsEditor();

protected:
	/// The utilities being viewed.
	BioXASUtilities *utilities_;

	/// The utilities status editor.
	BioXASControlEditor *statusEditor_;
	/// The shutters editor.
	BioXASControlEditor *shuttersEditor_;
	/// The valves editor.
	BioXASControlEditor *valvesEditor_;
	/// The ion pumps editor.
	BioXASControlEditor *ionPumpsEditor_;
	/// The flow switches editor.
	BioXASControlEditor *flowSwitchesEditor_;
	/// The pressure monitors editor.
	BioXASControlEditor *pressureMonitorsEditor_;
	/// The temperature monitors editor.
	BioXASControlEditor *temperatureMonitorsEditor_;
};

#endif // BIOXASUTILITIESVIEW_H
