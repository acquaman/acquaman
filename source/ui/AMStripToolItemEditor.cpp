#include "AMStripToolItemEditor.h"

AMStripToolItemEditor::AMStripToolItemEditor(AMStripToolItem *item, QWidget *parent) :
	QWidget(parent)
{
	// Initialize local variables.

	item_ = 0;

	// Create UI elements.

	QLabel *namePrompt = new QLabel("Name:");
	nameLabel_ = new QLabel();

	QLabel *descriptionPrompt = new QLabel("Description:");
	descriptionLineEdit_ = new QLineEdit();

	QLabel *colorPrompt = new QLabel("Color:");
	colorButton_ = new AMColorPickerButton(QColor());

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(namePrompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(nameLabel_, 0, 1);
	layout->addWidget(descriptionPrompt, 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(descriptionLineEdit_, 1, 1);
	layout->addWidget(colorPrompt, 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(colorButton_, 2, 1);

	setLayout(layout);

	// Current settings.

	setItem(item);
}

AMStripToolItemEditor::~AMStripToolItemEditor()
{

}

void AMStripToolItemEditor::setItem(AMStripToolItem *item)
{
	if (item_ != item) {

		if (item_) {
			disconnect( item_, 0, this, 0 );
		}

		item_ = item;

		if (item_) {
			connect( item_, SIGNAL(controlChanged(AMControl*)), this, SLOT(update()) );
		}

		refresh();

		emit itemChanged(item_);
	}
}

void AMStripToolItemEditor::clear()
{
	nameLabel_->clear();
	descriptionLineEdit_->clear();
	colorButton_->setColor(QColor());
}

void AMStripToolItemEditor::update()
{
	updateNameLabel();
	updateDescriptionLineEdit();
	updateColorButton();
}

void AMStripToolItemEditor::refresh()
{
	// Clear the view.

	clear();

	// Update view elements.

	update();
}

void AMStripToolItemEditor::updateNameLabel()
{
	QString text;

	if (item_ && item_->control())
		text = item_->control()->name();

	nameLabel_->setText(text);
}

void AMStripToolItemEditor::updateDescriptionLineEdit()
{
	QString text;
	bool enabled = false;

	if (item_ && item_->plotItem()) {
		text = item_->plotItem()->description();
		enabled = true;
	}

	descriptionLineEdit_->setText(text);
	descriptionLineEdit_->setEnabled(enabled);
}

void AMStripToolItemEditor::updateColorButton()
{
	QColor color;
	bool enabled = false;

	if (item_ && item_->plotItem()) {
		color = item_->plotItem()->linePen().color();
		enabled = true;
	}

	colorButton_->setColor(color);
	colorButton_->setEnabled(enabled);
}

void AMStripToolItemEditor::updateItemDescription()
{
	if (item_ && item_->plotItem()) {
		QString newDescription = descriptionLineEdit_->text();
		item_->plotItem()->setDescription(newDescription);
	}
}

void AMStripToolItemEditor::updateItemColor()
{
	if (item_ && item_->plotItem()) {
		QColor newColor = colorButton_->color();
		item_->plotItem()->setLinePen(QPen(newColor));
	}
}
