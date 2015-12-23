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

public slots:

protected slots:
	/// Hides all editors in the editors box.
	void hideEditors();

	/// Handles updating the view when a button is clicked.
	void onButtonClicked(QAbstractButton *button);

protected:
	/// The buttons group.
	QButtonGroup *buttons_;
	/// The editors box.
	QGroupBox *editorsBox_;
	/// The mapping between buttons and editors.
	QMap<QAbstractButton*, QWidget*> buttonEditorMap_;
};

#endif // BIOXASBEAMLINESTATUSBUTTONSVIEW_H
