#ifndef BIOXASUTILITIESVIEW_H
#define BIOXASUTILITIESVIEW_H

#include <QWidget>

class BioXASUtilities;
class CLSControlEditor;

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

protected:
	/// The utilities being viewed.
	BioXASUtilities *utilities_;

	/// The utilities status editor.
	CLSControlEditor *statusEditor_;
	/// The shutters editor.
	CLSControlEditor *shuttersEditor_;
	/// The valves editor.
	CLSControlEditor *valvesEditor_;
};

#endif // BIOXASUTILITIESVIEW_H
