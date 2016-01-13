#ifndef BIOXASFRONTENDSHUTTERSVIEW_H
#define BIOXASFRONTENDSHUTTERSVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASFrontEndShutters;
class AMExtendedControlEditor;

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
	void shuttersChanged(BioXASFrontEndShutters *newShutters);

public slots:
	/// Refreshes the view, clears and then updates it.
	void refresh();
	/// Sets the shutters being viewed.
	void setShutters(BioXASFrontEndShutters *shutters);

protected slots:
	/// Updates the upstream photon shutter editor.
	void updateUpstreamPhotonShutterEditor();
	/// Updates the downstream photon shutter editor.
	void updateDownstreamPhotonShutterEditor();
	/// Updates the safety shutter editor.
	void updateSafetyShutterEditor();

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
