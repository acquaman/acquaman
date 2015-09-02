#ifndef AMSTRIPTOOLITEMEDITOR_H
#define AMSTRIPTOOLITEMEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>

#include "ui/AMStripToolItem.h"
#include "ui/dataman/AMColorPickerButton.h"

class AMStripToolItemEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolItemEditor(AMStripToolItem *item, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripToolItemEditor();

	/// Returns the item being edited.
	AMStripToolItem* item() const { return item_; }

signals:
	/// Notifier that the strip tool item being edited has changed.
	void itemChanged(AMStripToolItem *newItem);

public slots:
	/// Sets the strip tool item being edited.
	void setItem(AMStripToolItem *item);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Clears and then updates the view.
	void refresh();

protected slots:
	/// Updates the name label to display the current item's pv name.
	void updateNameLabel();
	/// Updates the description line edit to display the current item's description.
	void updateDescriptionLineEdit();
	/// Updates the color button to display the current item's color.
	void updateColorButton();

	/// Updates the current item's description.
	void updateItemDescription();
	/// Updates the current item's color.
	void updateItemColor();

protected:
	/// The strip tool item being edited.
	AMStripToolItem *item_;

	/// The label displaying the item's pv name.
	QLabel *nameLabel_;
	/// The line edit displaying the item's description.
	QLineEdit *descriptionLineEdit_;
	/// The button displaying and editing the item's color.
	AMColorPickerButton *colorButton_;
};

#endif // AMSTRIPTOOLITEMEDITOR_H
