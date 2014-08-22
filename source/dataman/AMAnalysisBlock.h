/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMANALYSISBLOCK_H
#define AMANALYSISBLOCK_H

#include "dataman/database/AMDbObject.h"
#include "dataman/datasource/AMDataSource.h"

class QWidget;
class AMScan;

/// This class defines the interface for any data-processing operation object ("block") that can take 0 or more AMDataSources as input, and provide an AMDataSource output. By connecting AMDataSources and AMAnalysisBlocks together, a powerful data-processing chain can be created. The processing can take place in real-time, or operate in the background and notify when finished.
/*!
  <b>Using AMAnalysisBlocks:</b>

  AMAnalysisBlocks connect together to form a chain of data analysis operations.  They take their input from a list of AMDataSource inputs.  They also implement the AMDataSource interface themselves, so that they can be used directly as input for other blocks.  You connect blocks together by calling setInputDataSources() with a list of sources.

  Each block implementation can determine what consists of acceptable and sufficient input, by providing areInputDataSourcesAcceptable().  This might depend on the number of inputs, the dimensionality and size of each input, or any other property of the AMDataSources.  Depending on the block, it's possible for inputs to be optional OR required, or to accept a variable number of inputs. (For example, a summing analysis block could accept a variable number of inputs, as long as they all have the same dimensionality and size.)

  AMAnalysisBlocks can exist in an inactive/invalid state if they have no inputs. (This is the initial condition, on creation.) In this state, the AMDataSource::state() function should include the AMDataSource::InvalidFlag, and all requests to value() should return an invalid AMNumber.

  It's okay to use AMDataSources that are currently in the invalid state as input sources.  They simply remain connected, and the block implementation should watch for its inputs' stateChanged() signals so that when all inputs[1] <i>do</i> become valid, the block can start its processing and (now or eventually) become valid itself.  Other blocks using this block as input will follow.  See the possible AMDataSource::StateFlags for more information.

If you delete an AMDataSource (or AMAnalysisBlock) that is being used as input for another block, the remaining block will receive the deleted() signal and respond appropriately. The default behaviour (defined in AMAnalysisBlock::onInputSourceDeleted() is to discard ALL input sources and go into the inactive/invalid state, by calling setInputDataSourcesImplementation() with an empty list.)  This will cause the block to transition its state() to the invalid state, and all other blocks which depend on it will likewise follow. See? It's like a big happy game of Telephone!

[1] Or just some, depending on what the block needs to produce outputs.

<b>Implementing AMAnalysisBlocks</b>

To implement a real AMAnalysisBlock, you must implement the pure virtual functions found here, as well as the pure virtual functions in the AMDataSource interface which describe your output results.  (\sa AMDataSource for more details.)

If an analysis block requires parameters in addition to the input sources, implement custom functions to set and access these parameters. \todo Figure out if generic parameter-description and parameter-setting functionality is required in the interface/base-class definition.

Since AMAnalysisBlocks must be persistently stored and reloaded, they also inherit AMDbObject.  \sa AMDbObject for how to specify parameters so they are saved and reloaded from the database.

Finally, AMAnalysisBlocks may choose to implement a factory function to create QWidget editors for their unique parameters. This should return a newly-created  widget, connected to the block's parameter-setting functions. If you don't provide this functionality, the base version simply returns a null pointer.  \todo Define some standards for editor widgets (size range, preferred layout, etc.)
*/
class AMAnalysisBlock : public AMDbObject, public AMDataSource
{
	Q_OBJECT

	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(int rank READ rank)
	Q_PROPERTY(bool visibleInPlots READ visibleInPlots WRITE setVisibleInPlots)
	Q_PROPERTY(bool hiddenFromUsers READ hiddenFromUsers WRITE setHiddenFromUsers)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Analysis Block")
	Q_CLASSINFO("visibleInPlots", "upgradeDefault=true")
	Q_CLASSINFO("hiddenFromUsers", "upgradeDefault=false")

public:
	/// Create a new AMAnalysisBlock. The block is an AMDataSource of output data; \c outputName is the name for this AMDataSource.
	AMAnalysisBlock(const QString& outputName, QObject* parent = 0);
	/// Destructor.
	virtual ~AMAnalysisBlock();

	/// Both AMDbObject:: and AMDataSource:: have a name() function. Here we resolve that ambiguity.
	virtual QString name() const { return AMDataSource::name(); }

	/// Re-implemented from AMDataSource to call setModified().
	void setDescription(const QString &description) { AMDataSource::setDescription(description); setModified(true); }

	/// Set the input of this block as a list of data sources. Returns false if the inputs are not sufficient, or incompatible with this analysis block. It is okay to set AMDataSource inputs that are currently not isValid()... the calculation will simply start when they become valid.
	/*! Implementing classes should not re-implement this function; instead, they should provide setInputDataSourcesImplementation().

	This base-class function checks whether the inputs are acceptable, and if so, calls deregisterObserver() on the old sources, registerObserver() on the new sources, and calls the setInputDataSourcesImplementation() provided by the subclass.

	  \note An empty list MUST always be acceptable input data, indicating that this analysis block hasn't been connected yet, and is in the "inactive" or "invalid" state. In this condition, the output AMDataSource::state() should include the AMDataSource::InvalidFlag, and all value() requests return an invalid AMNumber.
	  */
	bool setInputDataSources(const QList<AMDataSource*>& dataSources);

	/// Check whether a set of data sources would be acceptable, compatible, and sufficient to provide input for this analysis block.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const = 0;


	/// Provides a very simple editor widget for inside AMDataSourcesEditor, which only lists the rank and size of the analysis block (and the value, for 0D analysis blocks only). Re-implement to provide custom editors.
	virtual QWidget* createEditorWidget();

	// Access to input data sources
	//////////////////////////

	/// Access the data sources which are currently providing input to this block
	virtual QList<AMDataSource*> inputDataSources() const = 0;
	/// Number of current input sources
	virtual int inputDataSourceCount() const = 0;
	/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
	virtual AMDataSource* inputDataSourceAt(int index) const = 0;
	/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name; if they do, this returns the first one.) Returns -1 if no input source found with this name.
	/*! This might be involve a slow lookup; users should not call repeatedly.*/
	virtual int indexOfInputSource(const QString& dataSourceName) const = 0;
	/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
	virtual int indexOfInputSource(const AMDataSource* source) const = 0;

	// Reimplemented from AMDataSource
	//////////////////////////////
	/// If implementing classes don't use the setState() method to notify of changes in their current output state, they must re-implement this function to reflect their actual current state.
	virtual int state() const { return state_;	}

	/// Specify that this data source should be visible (in plots and graphical displays).  Users are free to toggle this visibility.
	/*! Re-implemented from AMDataSource to call setModified().  */
	virtual void setVisibleInPlots(bool isVisible);

	/// Specify that this data source should be hidden from users by default. (ie: it contains some programming internals). This means that users shouldn't see it, or be able to toggle its visibility.
	/*! Re-implemented from AMDataSource to call setModified().  */
	virtual void setHiddenFromUsers(bool isHidden = true) { AMDataSource::setHiddenFromUsers(isHidden); setModified(true); }

	// New public functions
	//////////////////////////////

	/// If this analysis block is added to a scan, this can be used to access a pointer to that scan. [Some analysis blocks might need this to access calculation parameters that are part of the scan configuration or scan initial conditions.] If not part of a scan, returns 0.
	AMScan* scan() const { return scan_; }
	/// Used internally when this analysis block as added to a scan.
	void setScan(AMScan* scan) { scan_ = scan; }

public slots:
	/// Sets the name to both AMDbObject AND AMDataSource.
	virtual void setName(const QString &name);

protected slots:
	/// called automatically when a current input source is deleted. The default response is to discard ALL input sources and go into the invalid/inactive state. The base class implementation of this function is effectively the same as calling setInputDataSources() with an empty list. setInputDataSourcesImplementation() will be called with an empty list to tell the subclass to put itself in the invalid/inactive state.
	/*! If you re-implement this function, make sure to deregisterObserver() and disconnect the deleted() signal from the sources you no longer use. See the base class implementation for an example.*/
	virtual void onInputSourceDeleted(void* deletedSource);


signals:
	/// Emitted automatically by setInputDataSources() after this analysis block's input sources change.  This can be useful for editor widgets, etc. that need to update themselves based on the block's input sources, rather than the current state of the block itself.  (Changes to the block's <i>output<i> state, size, etc. are available from the AMDataSource::signalSource() signals.)
	void inputSourcesChanged();


protected:
	/// Implementing subclasses must provide a setInputDataSourcesImplementation(), which is called from setInputDataSources(). This will only be called if \c dataSources are acceptable and sufficient  (according to areInputDataSourcesAcceptable()), or if \c dataSources is empty, indicating the block is in the inactive/invalid state.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) = 0;

	/// Implementing classes can call this function whenever their state changes. It takes care of calling emitStateChanged() whenever the new state is different from the old, and sets the value that will be returned by the default implementation of state().
	void setState(int newState) {
		if(state_ != newState)
			emitStateChanged( (state_ = newState) );
	}


private:
	/// OR-combination of AMDataSource::StateFlag's returned by the default implementation of state().
	int state_;

	/// If added to a scan using AMScan::addAnalyzedDataSource(), contains a pointer to the scan.
	AMScan* scan_;

};

#endif // AMANALYSISBLOCK_H
