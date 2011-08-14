#ifndef AMCOLORPICKERBUTTON_H
#define AMCOLORPICKERBUTTON_H

#include <QToolButton>

class QColorDialog;

class AMColorPickerButton : public QToolButton
{
	Q_OBJECT
public:
	AMColorPickerButton(const QColor& initialColor, QWidget *parent = 0);
	explicit AMColorPickerButton(QWidget* parent = 0);

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
	/// Called when the color dialog is rejected (user has cancelled)
	void onColorDialogRejected();

protected:
	QColorDialog* colorDialog_;
	QColor currentColor_;

};

#endif // AMCOLORPICKERBUTTON_H
