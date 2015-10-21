#ifndef BIOXASVALVESVIEW_H
#define BIOXASVALVESVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

#include "beamline/BioXAS/BioXASValves.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASValvesView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValvesView(BioXASValves *valves, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesView();

	/// Returns the valves being viewed.
	BioXASValves* valves() const { return valves_; }

signals:
	/// Notifier that the valves being viewed have changed.
	void valvesChanged(BioXASValves *valves);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view with the lastest valves information.
	void update();
	/// Refreshes the view, clears and then updates it.
	void refresh();

	/// Sets the valves being viewed.
	void setValves(BioXASValves *valves);

protected:
	/// The valves being viewed.
	BioXASValves *valves_;

	/// The master valves editor.
	AMExtendedControlEditor *allValvesEditor_;
	/// The front end valves editors.
	AMExtendedControlEditor *frontEnd1_;
	AMExtendedControlEditor *frontEnd2_;
	AMExtendedControlEditor *frontEnd3_;
	AMExtendedControlEditor *frontEnd4_;
	AMExtendedControlEditor *frontEnd5_;
	AMExtendedControlEditor *frontEnd6_;
};

#endif // BIOXASVALVESVIEW_H
