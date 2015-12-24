#ifndef BIOXASBEAMLINESTATUSBAR_H
#define BIOXASBEAMLINESTATUSBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QLayout>
#include <QMap>

class AMExtendedControlEditor;
class BioXASShuttersButton;
class BioXASValvesButton;

class BioXASBeamlineStatusBar : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineStatusBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineStatusBar();

signals:
	/// Notifier that the selected button has changed.
	void selectedButtonChanged();

public slots:
	/// Refreshes the view.
	void refresh();

	/// Adds a new button with a corresponding view.
	void addButton(QAbstractButton *newButton, QWidget *view);
	/// Removes a button and its corresponding view.
	void removeButton(QAbstractButton *button);
	/// Clears all buttons and views.
	void clearButtons();

protected slots:
	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *newButton);

	/// Shows the view corresponding to the given button.
	void showViewForButton(QAbstractButton *button);
	/// Hides all views in the button views box.
	void hideViews();

	/// Handles updating the selected button when a button is clicked.
	void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selected button.
	QAbstractButton *selectedButton_;

	/// The buttons group.
	QButtonGroup *buttonsGroup_;
	/// The list of buttons.
	QList<QAbstractButton*> buttonsList_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;

	/// The mapping between buttons and corresponding views.
	QList<QWidget*> buttonViewsList_;
	/// The button views layout.
	QVBoxLayout *buttonViewsLayout_;
	/// The button views box.
	QGroupBox *buttonViewsBox_;
};

#endif // BIOXASBEAMLINESTATUSBAR_H
