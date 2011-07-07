#ifndef AMLINESERIESPROPERTYEDITOR_H
#define AMLINESERIESPROPERTYEDITOR_H

#include <QFrame>

namespace Ui {
	class AMLinePropertyEditor;
}

/// This widget provides an editor for pen settings (color, width, line style, fill enabled, and fill color).  At the moment, it only supports one-way communication: changes to its editor widgets result in the signals linePenChanged(), areaFilledChanged(), and areaFillBrushChanged(). However, there is no way to update its widgets programmatically -- it can only be user-driven.
class AMLinePropertyEditor : public QFrame
{
	Q_OBJECT
public:
	/// Default Constructor
	explicit AMLinePropertyEditor(QWidget *parent = 0);
	/// Construct an editor with its widgets initialized according to these settings
	AMLinePropertyEditor(const QPen& initialPenSettings, bool areaFilled = false, const QBrush& areaFillColor = Qt::darkGray, QWidget* parent = 0);
	/// Destructor
	~AMLinePropertyEditor();

signals:
	void linePenChanged(const QPen& newPenSettings);
	void areaFilledChanged(bool isFilled);
	void areaFillBrushChanged(const QBrush& newFillBrush);

public slots:


protected slots:
	/// Used internally to respond to user actions (color, width, or line style)
	void onPenSettingsChanged();
	/// Used internally to respond to user actions (area fill color changed)
	void onAreaFillColorChanged(const QColor& color);


protected:
	Ui::AMLinePropertyEditor* ui_;

};

#endif // AMLINESERIESPROPERTYEDITOR_H
