#ifndef ACQMAN_CHANNEL_H
#define ACQMAN_CHANNEL_H

#include <QObject>
#include <muParser/muParser.h>

#include "MPlot/MPlotSeriesData.h"

#include "dataman/AMDataTree.h"

// This class in an observer on AMDataTree's.
#include "AMObserver.h"

class AMScan;
class AMChannelSeriesData;

/// An AMChannel is a way of looking at raw data associated with a particular AMScan, as a 2D (or x-y data point) series.  The y-values of the channel can be set as any mathematical expression where the variables are the names of columns in the raw data (setExpression()).  The x-values can come directly from the x-data column (by default) or from a similar mathematical expression (setXExpression()).
class AMChannel : public QObject, public AMObserver {
Q_OBJECT
Q_PROPERTY(QString name READ name)

public:
	explicit AMChannel(AMScan* scan, const QString& name, const QString& expression, const QString& xExpression = "");

	virtual ~AMChannel() {
		dataTree()->observable()->removeObserver(this);
	}

    /// AMChannel name: (ex: "tey", "tfy_io", etc.)
    QString name() const { return name_; }

	/// The scan that owns this channel
	AMScan* scan() const { return scan_; }

	/// the expression currently set for this channel
	QString expression() const { return QString::fromStdString(parser_.GetExpr()); }
	/// The expression set for the x-values of this channel
	QString xExpression() const { if(defaultX_) return QString(); else return QString::fromStdString(parserX_.GetExpr()); }

	/// the number of accessible values in this series
	unsigned count() const { return dataTree()->count(); }

	/// the (y) value of this channel at index \c p
	double value(unsigned p) const;
	/// the x value of this channel at index \c p
	double x(unsigned p) const;

	/// Returns true if both expressions are set to valid expressions; the value() and x() will be what you asked for.
	bool isValid() const { return isValid_ && isValidX_; }

	/// If isValid() is false, returns an explanation of what's wrong.
	QString errorMsg() const;
	QString longErrorMsg() const;

	/// returns this channel's AMChannelSeriesData, which can be used for plotting within MPlot
	AMChannelSeriesData* seriesData() { return seriesData_; }

	/// returns the min/max values.  Warning: use only when count() >= 1.  Do not call on an empty channel. (\todo : the data tree columns already track min/max in an optimized way.  Is there any way to benefit from this, with an arbitrary expression?)
	double min() const { if(min_ == -1) searchMin(); return value(min_); }
	double max() const { if(min_ == -1) searchMax(); return value(max_); }
	double minX() const { if(minX_ == -1) searchMinX(); return x(minX_); }
	double maxX() const { if(maxX_ == -1) searchMaxX(); return x(maxX_); }

signals:
	/// \todo emit error messages and eror codes?

	/// Emitted when the raw underlying data or the expressions change ( \todo hookup to everything that needs it)
	void updated();

public slots:

	/// Set the expression:
	bool setExpression(const QString& expression);
	/// Set the expression for the x-axis values. (If \c xExpression is an empty string, the default x-data column of the scan is used directly.  This is faster than calculating an expression.)
	bool setXExpression(const QString& xExpression = "");

	/// Implementing AMObserver requirements:
public:
	virtual void onObservableChanged(AMObservable* source, int code, const char* msg);

protected:
	/// Name of this channel (Cannot be changed; used to retrieve channels from a scan with AMScan::channel(). )
    QString name_;

	/// Math expression parsers for evaluating channel values. parserX_ is used only if defaultX_ == false.
	mu::Parser parser_, parserX_;

	/// The AMScan associated with this channel.
	AMScan* scan_;

	/// The math expression parser gets optimized into bytecode, as long as the variable targets are not changed.  Therefore, this buffer holds the variable targets for the expression parser. The raw data column values are copied into here whenever value() is accessed.  The first element is the raw data x-column; remaining elements are the y columns of the raw data.
	mutable QVector<double> varStorage_;

	/// If defaultX_ == true, the raw data x-column is used directly for the x() value. (Faster performance than using an xExpression().)
	bool defaultX_;

	/// For optimization, this stores the raw data column indices that are actually used by the parsers.  -1 indicates the raw data x-column.
	QList<int> usedColumnIndices_, usedColumnIndicesX_;

	/// clears the parser variables and sets them to the names of the raw data columns
	bool setVariablesFromDataColumns();

	/// used to remember if the expressions were set successfully:
	bool isValid_, isValidX_;

	/// pointer to represetntation
	AMChannelSeriesData* seriesData_;

	/// max, min index tracking:
	mutable int min_, max_, minX_, maxX_;

	/// search for min/max and update the index trackers.  Warning: assumes count() >= 1. Do not call on an empty channel.
	void searchMin() const;
	void searchMax() const;
	void searchMinX() const;
	void searchMaxX() const;

public:
	/// access the data tree from scan object. \todo Should not be public. Why does it need to be to compile?  [friend of AMScan]
	AMDataTree* dataTree() const;


};

class AMChannelSeriesData : public MPlotAbstractSeriesData {
	Q_OBJECT

public:
	AMChannelSeriesData(AMChannel* channel, QObject* parent = 0) : MPlotAbstractSeriesData(parent) {
		channel_ = channel;
		connect(channel_, SIGNAL(updated()), this, SLOT(onFwdDataChanged()));
	}

	// Return the x-value (y-value) at index:
	virtual double x(unsigned index) const { return channel_->x(index); }
	virtual double y(unsigned index) const { return channel_->value(index); }

	// Return the number of elements
	virtual unsigned count() const { return channel_->count(); }


	// Return the bounds of the data (the rectangle containing the max/min x- and y-values)
	virtual QRectF boundingRect() const {
		if(channel_->count() == 0)
			return QRectF();
		else {
			double min = channel_->min();
			double max = channel_->max();
			double minX = channel_->minX();
			double maxX = channel_->maxX();
			return QRectF(QPointF(minX, min), QSizeF(maxX-minX, max-min));
		}
	}

signals:
	// Emit this when the data has been changed (will trigger a plot update)
	// The arguments let you specify the range of data that has been changed (inclusive)
	// To force an update of the entire plot, specify fromIndex > toIndex.
	void dataChanged(unsigned fromIndex, unsigned toIndex);

protected slots:
	void onFwdDataChanged() {
		emit dataChanged(1,0);
	}

protected:
	AMChannel* channel_;
};


#endif // CHANNEL_H
