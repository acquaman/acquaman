#ifndef BIOXASMASTERVALVESVIEW_H
#define BIOXASMASTERVALVESVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class BioXASMasterValves;
class BioXASValvesView;

class BioXASMasterValvesView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMasterValvesView(BioXASMasterValves *valves, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMasterValvesView();

	/// Returns the valves being viewed.
	BioXASMasterValves* valves() const { return valves_; }

signals:
	/// Notifier that the valves being viewed have changed.
	void valvesChanged(BioXASMasterValves *newValves);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the valves being viewed.
	void setValves(BioXASMasterValves *newValves);

protected slots:
	/// Updates the front-end valves view.
	void updateFrontEndView();
	/// Updates the Side valves view.
	void updateSideView();
	/// Updates the Main valves view.
	void updateMainView();
	/// Updates the Imaging valves view.
	void updateImagingView();

protected:
	/// The valves being viewed.
	BioXASMasterValves *valves_;

	/// The total valves editor.
	AMExtendedControlEditor *valvesEditor_;

	/// The front-end valves view.
	BioXASValvesView *frontEndView_;
	/// The Side valves view.
	BioXASValvesView *sideView_;
	/// The Main valves view.
	BioXASValvesView *mainView_;
	/// The Imaging valves view.
	BioXASValvesView *imagingView_;
};

#endif // BIOXASMASTERVALVESVIEW_H
