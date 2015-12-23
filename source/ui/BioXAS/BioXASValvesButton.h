#ifndef BIOXASVALVESBUTTON_H
#define BIOXASVALVESBUTTON_H

#include <QToolButton>

class BioXASValves;

class BioXASValvesButton : public QToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValvesButton(BioXASValves *valves, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesButton();

	/// Returns the valves.
	BioXASValves* valves() const { return valves_; }

signals:
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASValves *newValves);

public slots:
	/// Refreshes the button.
	void refresh();
	/// Sets the valves.
	void setValves(BioXASValves *newValves);

protected:
	/// The valves.
	BioXASValves *valves_;
};

#endif // BIOXASVALVESBUTTON_H
