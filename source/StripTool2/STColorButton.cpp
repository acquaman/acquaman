#include "STColorButton.h"

STColorButton::STColorButton(QWidget *parent) : QPushButton(parent)
{
    color_ = QColor();

    setMinimumWidth(50);

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

        if (color_.isValid())
            setStyleSheet("color: " + color_.name());

        emit colorChanged(color_);
    }

    repaint();
}

void STColorButton::selectColor()
{
    QColor selection = QColorDialog::getColor(color_, this);

    if (selection.isValid())
        setColor(selection);
}

void STColorButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

//    if (color_.isValid()) {

    /////////////////////////////////////////

//        QPalette p(palette());
//        p.setColor(QPalette::Button, color_);
//        setPalette(p);

    /////////////////////////////////////////

//        int colorPadding = 5;

//        QRect rect = event->rect();

//        QPainter painter(this);
//        painter.setBrush( QBrush(color_) );
//        painter.setPen("#CECECE");

//        rect.adjust(colorPadding, colorPadding, -1 - colorPadding, -1 - colorPadding);

//        painter.drawRect(rect);
//    }
}
