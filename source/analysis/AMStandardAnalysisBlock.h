#ifndef AMSTANDARDANALYSISBLOCK_H
#define AMSTANDARDANALYSISBLOCK_H

#include "dataman/AMAnalysisBlock.h"

/// This class provides many of the "boiler-plate" functions required for an AMAnalysisBlock, including managing the input data sources and representing the output AMDataSource axes.  Subclasses can make use of it by using the protected variables sources_ and axes_ to store their input data sources and output axes, respectively.
class AMStandardAnalysisBlock : public AMAnalysisBlock
{
	Q_OBJECT
public:
	/// Create a new AMAnalysisBlock. The block is also an AMDataSource of output data; \c outputName is the name for this AMDataSource.
	AMStandardAnalysisBlock(const QString& outputName, QObject *parent = 0);

	// Access to input data sources
	//////////////////////////

	/// Access the data sources which are currently providing input to this block
	virtual QList<AMDataSource*> inputDataSources() const { return sources_; }

	/// Number of current input sources
	virtual int inputDataSourceCount() const { return sources_.count(); }

	/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
	virtual AMDataSource* inputDataSourceAt(int index) const { if(index<0 || index>=sources_.count()) return 0; return sources_.at(index); }

	/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name; if they do, this returns the first one.) Returns -1 if no input source found with this name.
	/*! This might be involve a slow lookup; users should not call repeatedly.*/
	virtual int indexOfInputSource(const QString& dataSourceName) const {
		for(int i=0; i<inputDataSourceCount(); i++)
			if(inputDataSourceAt(i)->name() == dataSourceName)
				return i;
		return -1;
	}

	/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
	virtual int indexOfInputSource(const AMDataSource* source) const {
		for(int i=0; i<inputDataSourceCount(); i++)
			if(inputDataSourceAt(i) == source)
				return i;
		return -1;
	}


	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { return axes_; }

	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return axes_.count(); }

	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const {
		int r = axes_.count();
		AMnDIndex rv(r, false);
		for(int i=0; i<r; i++)
			rv[i] = axes_.at(i).size;
		return rv;
	}

	/// Returns the size along a single axis \c axisNumber. This should be fast.
	virtual int size(int axisId) const {
		// if((unsigned)axisId >= (unsigned)axes_.count())
		//	return 0;
		return axes_.at(axisId).size;
	}

	/// Returns a bunch of information about a particular axis.
	virtual AMAxisInfo axisInfoAt(int axisId) const {
		// if((unsigned)axisId >= (unsigned)axes_.count())
		// 	return AMAxisInfo("invalid", 0, "invalid axis");
		return axes_.at(axisId);
	}

	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.
	virtual int idOfAxis(const QString& axisName) {
		for(int i=0; i<axes_.count(); i++)
			if(axes_.at(i).name == axisName)
				return i;
		return -1;
	}


signals:

public slots:


protected:
	QList<AMDataSource*> sources_;
	QList<AMAxisInfo> axes_;


};

#endif // AMSTANDARDANALYSISBLOCK_H
