#ifndef AMCOLORPICKERBUTTON2_H
#define AMCOLORPICKERBUTTON2_H

#include <QToolButton>

class QColorDialog;

class AMColorPickerButton2 : public QToolButton
{
	Q_OBJECT
public:
    explicit AMColorPickerButton2(const QColor& initialColor = Qt::black, QWidget *parent = 0);

	/// Retrieve the color currently displayed/selected
	QColor color() const { return currentColor_; }

signals:
	/// Emitted when the current color changes
	void colorChanged(const QColor& newColor);

public slots:

	/// Call to set the color currently displayed/selected
	void setColor(const QColor& newColor);

	/// Called either on click, or programmatically, to show the color browser and choose a new color.
	void activateColorBrowser();

protected slots:
	/// Called when the color dialog is finished (user has selected a color)
	void onColorDialogAccepted();

protected:
	QColorDialog* colorDialog_;
	QColor currentColor_;

};

#endif // AMCOLORPICKERBUTTON_H
