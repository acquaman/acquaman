#ifndef BIOXASFRONTENDSHUTTERSVIEW_H
#define BIOXASFRONTENDSHUTTERSVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASFrontEndShuttersView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndShuttersView(BioXASFrontEndShutters *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShuttersView();

	/// Returns the shutters being viewed.
	BioXASFrontEndShutters* shutters() const { return shutters_; }

signals:
	/// Notifier that the shutters being viewed have changed.
	void shuttersChanged(AMControl *newShutters);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view, clears and then updates it.
	void refresh();

	/// Sets the shutters being viewed.
	void setShutters(BioXASFrontEndShutters *shutters);

protected:
	/// The shutters being viewed.
	BioXASFrontEndShutters *shutters_;

	/// The shutters editor.
	AMExtendedControlEditor *shuttersEditor_;
	/// The upstream photon shutter editor.
	AMExtendedControlEditor *psh1_;
	/// The downstream photon shutter editor.
	AMExtendedControlEditor *psh2_;
	/// The safety shutter editor.
	AMExtendedControlEditor *ssh_;
};

#endif // BIOXASFRONTENDSHUTTERSVIEW_H
