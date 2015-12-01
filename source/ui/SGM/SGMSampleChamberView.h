#ifndef SGMSAMPLECHAMBERVIEW_H
#define SGMSAMPLECHAMBERVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;
class SGMSampleChamber;

class SGMSampleChamberView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberView(SGMSampleChamber *sampleChamber, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberView();

	/// Returns the sample chamber being viewed.
	SGMSampleChamber* sampleChamber() const { return sampleChamber_; }

signals:
	/// Notifier that the sample chamber being viewed has changed.
	void sampleChamberChanged(SGMSampleChamber *newSampleChamber);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the sample chamber being viewed.
	void setSampleChamber(SGMSampleChamber *newSampleChamber);

protected slots:
	/// Clears the view.
	void clear();

	/// Updates the vacuum editor.
	void updateVacuumEditor();
	/// Updates the door editor.
	void updateDoorEditor();
	/// Updates the pressure editor.
	void updatePressureEditor();
	/// Updates the ion gauge editor.
	void updateIonGaugeEditor();
	/// Updates the leak valve editor.
	void updateLeakValveEditor();
	/// Updates the roughing pump editor.
	void updateRoughingPumpEditor();
	/// Clears the turbos view.
	void clearTurbosView();
	/// Refreshes the turbos view.
	void refreshTurbosView();

protected:
	/// The sample chamber being viewed.
	SGMSampleChamber *sampleChamber_;

	/// The vacuum editor.
	AMExtendedControlEditor *vacuumEditor_;
	/// The door status editor.
	AMExtendedControlEditor *doorEditor_;
	/// The pressure editor.
	AMExtendedControlEditor *pressureEditor_;
	/// The pressure ion gauge editor.
	AMExtendedControlEditor *ionGaugeEditor_;
	/// The VAT valve view.
	AMExtendedControlEditor *leakValveEditor_;
	/// The roughing pump editor.
	AMExtendedControlEditor *roughingPumpEditor_;
	/// The sample chamber turbos view layout.
	QHBoxLayout *turbosViewLayout_;
	/// The list of extended control editors for the turbos
	QList<AMExtendedControlEditor*> turboEditorsList_;
};

#endif // SGMSAMPLECHAMBERVIEW_H
