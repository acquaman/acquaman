#ifndef CLSBUTTONBAR_H
#define CLSBUTTONBAR_H

#include <QWidget>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QLayout>

///
/// The button bar is a widget which contains a group of buttons.
///

class CLSButtonBar : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
	Q_ENUMS(SelectionMode)

	Q_PROPERTY(ReselectionMode reselectionMode READ reselectionMode WRITE setReselectionMode NOTIFY reselectionModeChanged)
	Q_ENUMS(ReselectionMode)

public:
	/// Enumeration of possible selection behaviors (initially unselected).
	enum SelectionMode { UnselectedUncheckedToSelectedUnchecked = 0, UnselectedUncheckedToSelectedChecked = 1 };
	/// Enumeration of possible reselection behaviors (initially selected).
	enum ReselectionMode { SelectedUncheckedToSelectedUnchecked = 0, SelectedCheckedToUnselectedUnchecked = 1 };

	/// Constructor.
	explicit CLSButtonBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSButtonBar();

	/// Returns the selected button.
	QAbstractButton* selectedButton() const { return selectedButton_; }

	/// Returns the selection mode.
	SelectionMode selectionMode() const { return selectionMode_; }
	/// Returns the reselection mode.
	ReselectionMode reselectionMode() const { return reselectionMode_; }

signals:
	/// Notifier that the buttons have changed.
	void buttonsChanged();
	/// Notifier that the button was selected.
	void selectedButtonChanged(QAbstractButton *newButton);
	/// Notifier that a button was clicked.
	void buttonClicked(QAbstractButton *clickedButton);
	/// Notifier that the selection mode has changed.
	void selectionModeChanged(SelectionMode newMode);
	/// Notifier that the reselection mode has changed.
	void reselectionModeChanged(ReselectionMode newMode);

public slots:
	/// Adds a button.
	void addButton(QAbstractButton *newButton);
	/// Removes a button.
	void removeButton(QAbstractButton *button);
	/// Removes all buttons, without deleting them.
	void removeButtons();
	/// Removes and deletes all buttons.
	void clearButtons();

	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *button);

	/// Sets the selection mode.
	void setSelectionMode(SelectionMode newMode);
	/// Sets the reselection mode.
	void setReselectionMode(ReselectionMode newMode);

protected slots:
	/// Handles updating the selected button, in response to a button being clicked.
	void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selection mode, governs button behavior when clicked after initially unselected.
	SelectionMode selectionMode_;
	/// The reselection mode, governs button behavior when clicked after initally selected.
	ReselectionMode reselectionMode_;

	/// The selected button.
	QAbstractButton *selectedButton_;
	/// The buttons group.
	QButtonGroup *buttonsGroup_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
};

#endif // CLSBUTTONBAR_H
