#ifndef ROIPLOTMARKER_H
#define ROIPLOTMARKER_H

#include "MPlot/MPlotItem.h"

class ROIPlotMarker : public MPlotItem
{
public:
	/// Constructor.  Requires the original geometry of the ROI to set correctly.
	ROIPlotMarker(QString name = "", double center = 0, double low = 0, double high = 0, double height = 1000, double sx = 1, double sy = 1);

	/// Returns the description of the marker.
	QString description() const { return name_; }
	/// Set the description of the marker.
	void setDescription(const QString &description) { name_ = description; }
	/// Returns the position of the center line.
	double center() const { return center_*sx_; }
	/// Sets the center position of marker.
	void setCenter(double position) { center_ = position; updatePlot(); }
	/// Returns the lower end of the boundingRect.
	double lowEnd() const { return low_*sx_; }
	/// Sets the position of the lower end of the boundingRect.
	void setLowEnd(double position) { low_ = position; updatePlot(); }
	/// Returns the higher end of the boundingRect.
	double highEnd() const { return high_*sx_; }
	/// Sets the position of the higher end of the boundingRect.
	void setHighEnd(double position) { high_ = position; updatePlot(); }
	/// Returns the height of the boundingRect.  This CANNOT be zero and thus if the height is zero then it returns 1.
	double height() const { if (height_ == 0) return 1; else return height_*sy_; }
	/// Sets the height of the boundingRect.
	void setHeight(double height) { height_ = height; }
	/// Returns the current scaling factor for the x-axis.
	double xScale() { return sx_; }
	/// Returns the current scaling factor for the y-axis.
	double yScale() { return sy_; }
	/// Sets the scaling factors for the width and height.
	void setScaling(double sx = 1, double sy = 1) { sx_ = sx; sy_ = sy; }
	/// Sets whether the marker has been "selected" or not.  Changes the colour of the marker here.
	void setHighlighted(bool highlight);
	/// Returns whether the marker is highlighted or not.
	bool getHighlighted() { return isHighlighted_; }

	/// Bounding rect: reported in our PlotItem coordinates, which are just the actual data coordinates. This is used by the graphics view system to figure out how much we cover/need to redraw.  Subclasses that draw selection borders or markers need to add their size on top of this.
	virtual QRectF boundingRect() const;

	/// Data rect: also reported in our PlotItem coordinates, which are the actual data coordinates. This is used by the auto-scaling to figure out the range of our data on an axis.
	virtual QRectF dataRect() const;

	/// Paint: Draws a semi-transparent beam to show the width of a particular region of interest.
	virtual void paint(QPainter* painter,
					   const QStyleOptionGraphicsItem* option,
					   QWidget* widget);

	/// Updates the plot to show the new size and shape of the marker.
	virtual void updatePlot() { prepareGeometryChange(); emitBoundsChanged(); emitLegendContentChanged(); if (low_ == -1 && high_ == -1) hide(); else show(); }

	/// The color used to represent this plot item in the legend.  Subclasses can re-implement this for more detail.
	virtual QBrush legendColor() const { return QBrush(QColor(0, 0, 0)); }

private:
	QString name_;
	double center_;
	double low_;
	double high_;
	double height_;
	double sx_;
	double sy_;

	/// If the ROI is "selected" in the ROI list, then the marker is to change colour.  This keeps track to ensure that the colour of the marker is correct.
	bool isHighlighted_;
	QColor markerColor_;
};

#endif // ROIPLOTMARKER_H
