#ifndef BIOXASBUTTONBAR_H
#define BIOXASBUTTONBAR_H

#include <QWidget>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QLayout>

class BioXASButtonBar : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASButtonBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASButtonBar();

	/// Returns the selected button.
	QAbstractButton* selectedButton() const { return selectedButton_; }

signals:
	/// Notifier that the buttons have changed.
	void buttonsChanged();
	/// Notifier that the button was selected.
	void selectedButtonChanged(QAbstractButton *newButton);

public slots:
	/// Adds a button.
	void addButton(QAbstractButton *newButton);
	/// Removes a button.
	void removeButton(QAbstractButton *button);
	/// Clears all buttons.
	void clearButtons();

protected slots:
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

#endif // BIOXASBUTTONBAR_H
