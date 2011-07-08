#ifndef AMIMAGEPROPERTYEDITOR_H
#define AMIMAGEPROPERTYEDITOR_H

#include <QFrame>
#include "MPlot/MPlotColorMap.h"

namespace Ui {
	class AMImagePropertyEditor;
}


/// This widget provides an editor for color map settings (standard color map, custom colors, brightness, contrast, gamma).  At the moment, it only supports one-way communication: changes to its editor widgets result in the signals useStandardColorMapChanged(), standardColorMapChanged(), firstColorChanged(), secondColorChanged(), brightnessChanged(), contrastChanged(), and gammaChanged(). However, there is no way to update its widgets programmatically -- it can only be user-driven.
class AMImagePropertyEditor : public QFrame
{
	Q_OBJECT

public:
	explicit AMImagePropertyEditor(QWidget *parent = 0);
	AMImagePropertyEditor(bool useStandardColorMap, int mplotStandardColorMap, const QColor& firstColor, const QColor& secondColor, double brightness, double contrast, double gamma, QWidget* parent = 0);
	AMImagePropertyEditor(const MPlotColorMap& initialMap, QWidget* parent = 0);
	~AMImagePropertyEditor();

	bool useStandardColorMap() const;
	int standardColorMap() const;
	QColor firstColor() const;
	QColor secondColor() const;

	double brightness() const { return brightness_; }
	double contrast() const { return contrast_; }
	double gamma() const { return gamma_; }

signals:
	void useStandardColorMapChanged(bool);
	void standardColorMapChanged(int mplotStandardColorMap);
	void firstColorChanged(const QColor& color);
	void secondColorChanged(const QColor& color);

	void brightnessChanged(double brightness);
	void contrastChanged(double contrast);
	void gammaChanged(double gamma);

	// this cumulative signal emits an updated color map anytime anything changes
	void colorMapChanged(const MPlotColorMap& newColorMap);

protected slots:
	void onBrightnessSliderChanged(int sliderValue);
	void onContrastSliderChanged(int sliderValue);
	void onGammaSliderChanged(int sliderValue);

	void onUseStandardColorMapChanged(bool useStandard);
	void onStandardColorMapChanged(int index);
	void onFirstColorChanged(const QColor& color);
	void onSecondColorChanged(const QColor& color);

protected:
	Ui::AMImagePropertyEditor* ui_;

	MPlotColorMap currentMap_;

	double brightness_, contrast_, gamma_;
};

#endif // AMIMAGEPROPERTYEDITOR_H
