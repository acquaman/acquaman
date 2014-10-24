#ifndef STCOLORBUTTON_H
#define STCOLORBUTTON_H

#include "AMQEvents.h"

#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QRect>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

class STColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit STColorButton(QWidget *parent = 0);
    virtual ~STColorButton();

    QColor color() const;

signals:
    void colorChanged(const QColor &newColor);

public slots:
    void setColor(const QColor &newColor);
    void selectColor();
    void paintEvent(QPaintEvent *event);

protected:
    QColor color_;

};

#endif // STCOLORBUTTON_H
