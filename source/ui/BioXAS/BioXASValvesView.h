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
	AMExtendedControlEditor *allValves_;

	/// The front end valves editors.
	AMExtendedControlEditor *frontEndValves_;
	AMExtendedControlEditor *frontEnd1_;
	AMExtendedControlEditor *frontEnd2_;
	AMExtendedControlEditor *frontEnd3_;
	AMExtendedControlEditor *frontEnd4_;
	AMExtendedControlEditor *frontEnd5_;
	AMExtendedControlEditor *frontEnd6_;

	/// The Side valves editors.
	AMExtendedControlEditor *sideValves_;
	AMExtendedControlEditor *side1_;
	AMExtendedControlEditor *side2_;
	AMExtendedControlEditor *side3_;
	AMExtendedControlEditor *side4_;
	AMExtendedControlEditor *side5_;

	/// The Main valves editors.
	AMExtendedControlEditor *mainValves_;
	AMExtendedControlEditor *main1_;
	AMExtendedControlEditor *main2_;
	AMExtendedControlEditor *main3_;
	AMExtendedControlEditor *main4_;
	AMExtendedControlEditor *main5_;

	/// The Imaging valves editors.
	AMExtendedControlEditor *imagingValves_;
	AMExtendedControlEditor *imaging1_;
	AMExtendedControlEditor *imaging2_;
	AMExtendedControlEditor *imaging3_;
	AMExtendedControlEditor *imaging4_;
	AMExtendedControlEditor *imaging5_;
	AMExtendedControlEditor *imaging6_;
	AMExtendedControlEditor *imaging7_;
	AMExtendedControlEditor *imaging8_;
};

#endif // BIOXASVALVESVIEW_H
