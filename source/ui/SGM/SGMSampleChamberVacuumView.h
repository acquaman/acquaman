#ifndef SGMSAMPLECHAMBERVACUUMVIEW_H
#define SGMSAMPLECHAMBERVACUUMVIEW_H

#include <QWidget>

#include "source/beamline/SGM/SGMSampleChamberVacuum.h"

#include "source/ui/beamline/AMExtendedControlEditor.h"
#include "source/ui/SGM/SGMVATValveView.h"

class SGMSampleChamberVacuumView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumView(SGMSampleChamberVacuum *vacuumControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumView();

	/// Returns the vacuum control being viewed.
	SGMSampleChamberVacuum* vacuum() const { return vacuum_; }

signals:
	/// Notifier that the vacuum control being viewed has changed.
	void vacuumChanged(SGMSampleChamberVacuum *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the vacuum control being viewed.
	void setVacuum(SGMSampleChamberVacuum *newControl);

protected slots:
	/// Clears the turbos view.
	void clearTurbosView();

	/// Refreshes the vacuum editor.
	void refreshVacuumEditor();
	/// Refreshes the pressure editor.
	void refreshPressureEditor();
	/// Refreshes the VAT valve view.
	void refreshVATValveView();
	/// Refreshes the turbos view.
	void refreshTurbosView();

protected:
	/// The vacuum control being viewed.
	SGMSampleChamberVacuum *vacuum_;

	/// The vacuum control editor.
	AMExtendedControlEditor *vacuumEditor_;
	/// The pressure editor.
	AMExtendedControlEditor *pressureEditor_;
	/// The VAT valve view.
	SGMVATValveView *vatValveView_;

	/// The sample chamber turbos view layout.
	QHBoxLayout *turbosViewLayout_;
	/// The list of extended control editors for the turbos
	QList<AMExtendedControlEditor*> turboEditorsList_;
};

#endif // SGMSAMPLECHAMBERVACUUMVIEW_H
