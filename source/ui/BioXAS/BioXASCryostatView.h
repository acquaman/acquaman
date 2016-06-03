#ifndef BIOXASCRYOSTATVIEW_H
#define BIOXASCRYOSTATVIEW_H

#include <QWidget>

class BioXASCryostat;
class CLSControlEditor;

class BioXASCryostatView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCryostatView(BioXASCryostat *cryostat, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCryostatView();

	/// Returns the cryostat being viewed.
	BioXASCryostat* control() const { return cryostat_; }

signals:
	/// Notifier that the cryostat being viewed has changed.
	void controlChanged(BioXASCryostat *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the cryostat being viewed.
	void setControl(BioXASCryostat *newControl);

protected slots:
	/// Updates the pressure editor.
	void updatePressureEditor();
	/// Updates the temperature editor.
	void updateTemperatureEditor();

protected:
	/// The cryostat being viewed.
	BioXASCryostat *cryostat_;
	/// The pressure editor.
	CLSControlEditor *pressureEditor_;
	/// The temperature editor.
	CLSControlEditor *temperatureEditor_;
};

#endif // BIOXASCRYOSTATVIEW_H
