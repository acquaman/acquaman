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

public:
	/// Constructor.
	explicit CLSButtonBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSButtonBar();

	/// Returns the selected button.
	QAbstractButton* selectedButton() const { return selectedButton_; }
	/// Returns the buttons.
	QList<QAbstractButton*> buttons() const;

signals:
	/// Notifier that the button was selected.
	void selectedButtonChanged(QAbstractButton *newButton);
	/// Notifier that the buttons have changed.
	void buttonsChanged();
	/// Notifier that a button has been clicked.
	void buttonClicked(QAbstractButton *button);

public slots:
	/// Sets the selected button.
	void setSelectedButton(QAbstractButton *button);

	/// Adds a button.
	void addButton(QAbstractButton *newButton);
	/// Removes a button.
	void removeButton(QAbstractButton *button);
	/// Removes all buttons, without deleting them.
	void removeButtons();
	/// Removes and deletes all buttons.
	void clearButtons();

protected slots:
	/// Handles updating the selected button, in response to a button being clicked.
	virtual void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// The selected button.
	QAbstractButton *selectedButton_;

	/// The buttons group.
	QButtonGroup *buttonsGroup_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
};

#endif // CLSBUTTONBAR_H
