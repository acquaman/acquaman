#ifndef AMCOLOREDTEXTTOOLBUTTON_H
#define AMCOLOREDTEXTTOOLBUTTON_H

#include <QToolButton>

/// This is a very simple subclass that facilitates setting the text color of a QToolButton
class AMColoredTextToolButton : public QToolButton
{
	Q_OBJECT
public:
	/// Construct a QToolButton with the given \c textColor
	explicit AMColoredTextToolButton(const QColor& textColor = Qt::black, QWidget *parent = 0);

	/// Returns the current text color
	QColor textColor() const { return textColor_; }


signals:

public slots:
	void setTextColor(const QColor& color);

protected:
	QColor textColor_;

};

#endif // AMCOLOREDTEXTTOOLBUTTON_H
