#ifndef BIOXASBEAMLINESTATUSBUTTONSVIEW_H
#define BIOXASBEAMLINESTATUSBUTTONSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QLayout>
#include <QMap>

class AMExtendedControlEditor;
class BioXASShuttersButton;
class BioXASValvesButton;

class BioXASBeamlineStatusButtonsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASBeamlineStatusButtonsView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineStatusButtonsView();

signals:
	/// Notifier that the selected button has changed.
	void selectedButtonChanged();

public slots:
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *newButton);

	/// Shows the editor corresponding to the given button.
	void showEditorForButton(QAbstractButton *button);
	/// Hides all editors in the editors box.
	void hideEditors();

	/// Handles updating the view when a button is clicked.
	void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selected button.
	QAbstractButton *selectedButton_;
	/// The buttons group.
	QButtonGroup *buttons_;
	/// The editors box.
	QGroupBox *editorsBox_;
	/// The mapping between buttons and editors.
	QMap<QAbstractButton*, QWidget*> buttonEditorMap_;
};

#endif // BIOXASBEAMLINESTATUSBUTTONSVIEW_H
