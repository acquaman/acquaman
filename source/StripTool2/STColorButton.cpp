#include "STColorButton.h"

STColorButton::STColorButton(QWidget *parent) : QPushButton(parent)
{
	color_ = QColor();
	connect( this, SIGNAL(clicked()), this, SLOT(selectColor()) );
}

STColorButton::~STColorButton()
{

}

QColor STColorButton::color() const
{
	return color_;
}

void STColorButton::setColor(const QColor &newColor)
{
	if (color_ != newColor) {
		color_ = newColor;

		if (color_.isValid()) {
			QPalette pal = this->palette();
			pal.setColor(QPalette::Button, color_);
			setAutoFillBackground(true);
			setPalette(pal);
		}

		emit colorChanged(color_);
	}

	update();
}

void STColorButton::selectColor()
{
	QColor selection = QColorDialog::getColor(color_, this);

	if (selection.isValid())
		setColor(selection);
}

void STColorButton::paintEvent(QPaintEvent *event)
{

	Q_UNUSED(event)

	/*
	QStyleOptionButton option;
	option.initFrom(this);
	option.state = isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
	if (isDefault())
		option.features |= QStyleOptionButton::DefaultButton;
	option.text = text();
	option.icon = icon();

	QPalette palette(this->palette());
	palette.setBrush(QPalette::Background, QBrush(color_));
	setPalette(palette);
	option.backgroundColor = this->palette().color(QPalette::Background);

	QPalette palette(palette());
	palette.setBrush(QPalette::Background, QBrush(color_));
	setPalette(palette);

	QPainter painter(this);
	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);


	QPushButton::paintEvent(event);

	if (color_.isValid()) {

		///////////////////////////////////////

		QPalette p(palette());
		p.setColor(QPalette::Button, color_);
		setPalette(p);

		///////////////////////////////////////

		int colorPadding = 5;

		QRect rect = event->rect();

		QPainter painter(this);
		painter.setBrush( QBrush(color_) );
		painter.setPen("#CECECE");

		rect.adjust(colorPadding, colorPadding, -1 - colorPadding, -1 - colorPadding);

		painter.drawRect(rect);
	}
	*/
}
