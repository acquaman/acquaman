#ifndef SGMSAMPLECHAMBERVACUUMVIEW_H
#define SGMSAMPLECHAMBERVACUUMVIEW_H

#include <QWidget>

#include "beamline/SGM/SGMSampleChamberVacuum.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/SGM/SGMVATValveView.h"

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
	/// Refreshes the vacuum editor.
	void refreshVacuumEditor();
	/// Refreshes the pressure editor.
	void refreshPressureEditor();
	/// Refreshes the VAT valve view.
	void refreshVATValveView();

protected:
	/// The vacuum control being viewed.
	SGMSampleChamberVacuum *vacuum_;

	/// The vacuum control editor.
	AMExtendedControlEditor *vacuumEditor_;
	/// The pressure editor.
	AMExtendedControlEditor *pressureEditor_;
	/// The VAT valve view.
	SGMVATValveView *vatValveView_;
};

#endif // SGMSAMPLECHAMBERVACUUMVIEW_H
