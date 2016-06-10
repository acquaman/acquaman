#ifndef CLSBUTTONBAR_H
#define CLSBUTTONBAR_H

#include <QWidget>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QLayout>

///
/// The button bar is a widget which contains a group of buttons.
/// Those buttons can work in exclusive mode or not
///

class CLSButtonBar : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
	Q_ENUMS(SelectionMode)

public:
	/// Enumeration of the different click-selection modes.
	enum SelectionMode { ClickTogglesSelection = 0, ClickAlwaysSelects = 1 };

	/// Constructor.
	explicit CLSButtonBar(SelectionMode mode = ClickTogglesSelection, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSButtonBar();

	/// Returns the selection mode.
	SelectionMode selectionMode() const { return selectionMode_; }
	/// Returns the selected button.
	QAbstractButton* selectedButton() const { return selectedButton_; }
	/// Returns the buttons.
	QList<QAbstractButton*> buttons() const;

signals:
	/// Notifier that the selection mode has changed.
	void selectionModeChanged(SelectionMode newMode);
	/// Notifier that the button was selected.
	void selectedButtonChanged(QAbstractButton *newButton);
	/// Notifier that the buttons have changed.
	void buttonsChanged();
	/// Notifier that a button has been clicked.
	void buttonClicked(QAbstractButton *button);

public slots:
	/// Sets the selection mode.
	void setSelectionMode(SelectionMode newMode);
	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *button);

	/// Adds a button.
	void addButton(QAbstractButton *newButton);
	/// Removes a button.
	void removeButton(QAbstractButton *button);
	/// Clears all buttons.
	void clearButtons();

protected slots:
	/// Handles updating the selected button, in response to a button being clicked.
	virtual void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selection mode.
	SelectionMode selectionMode_;
	/// The selected button.
	QAbstractButton *selectedButton_;

	/// The buttons group.
	QButtonGroup *buttonsGroup_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
};

#endif // CLSBUTTONBAR_H
