#ifndef BIOXASSHUTTERSBUTTON_H
#define BIOXASSHUTTERSBUTTON_H

#include <QToolButton>

class BioXASShutters;

class BioXASShuttersButton : public QToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShuttersButton(BioXASShutters *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersButton();

	/// Returns the shutters.
	BioXASShutters* shutters() const { return shutters_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged(BioXASShutters *newShutters);

public slots:
	/// Refreshes the button.
	void refresh();
	/// Sets the shutters.
	void setShutters(BioXASShutters *newShutters);

protected:
	/// The shutters.
	BioXASShutters *shutters_;
};

#endif // BIOXASSHUTTERSBUTTON_H
