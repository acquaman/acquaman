#ifndef BIOXASVALVESVIEW_H
#define BIOXASVALVESVIEW_H

#include <QWidget>
#include <QLayout>

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
	/// Refreshes the view, clears and then updates it.
	void refresh();
	/// Sets the valves being viewed.
	void setValves(BioXASValves *valves);

protected:
	/// Clears the child valves.
	void clearChildValves();
	/// Refreshes the child valves.
	void refreshChildValves();

protected:
	/// The valves being viewed.
	BioXASValves *valves_;

	/// The valves editor.
	AMExtendedControlEditor *valvesEditor_;

	/// The child valves layout.
	QVBoxLayout *childValvesLayout_;
	/// The list of child valves editors.
	QList<AMExtendedControlEditor*> childValvesEditors_;
};

#endif // BIOXASVALVESVIEW_H
