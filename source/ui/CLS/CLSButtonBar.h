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

public:
	/// Constructor.
	explicit CLSButtonBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSButtonBar();

	/// Returns the selected button.
	QAbstractButton* selectedButton() const { return selectedButton_; }

signals:
	/// Notifier that the buttons have changed.
	void buttonsChanged();
	/// Notifier that the button was selected.
	void selectedButtonChanged(QAbstractButton *newButton);

protected slots:
	/// Adds a button.
	void addButton(QAbstractButton *newButton);
	/// Removes a button.
	void removeButton(QAbstractButton *button);
	/// Clears all buttons.
	void clearButtons();

	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *button);

	/// Handles updating the selected button, in response to a button being clicked.
	void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selected button.
	QAbstractButton *selectedButton_;
	/// The buttons group.
	QButtonGroup *buttonsGroup_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
};

#endif // CLSBUTTONBAR_H
