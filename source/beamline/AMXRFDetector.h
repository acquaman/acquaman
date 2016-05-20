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


#ifndef AMXRFDETECTOR_H
#define AMXRFDETECTOR_H

#include "beamline/AMDetector.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"
#include "dataman/AMRegionOfInterest.h"
#include "util/AMEmissionLine.h"

#include <QSignalMapper>
#define AMXRFDETECTOR_SPECTRUMSIZE_DEADTIMESIZE_MISMATCH 678000

/// This is the new base class that all XRF detectors should inherit from.  It should contain all the necessary prerequisites for a quick start up and also provide a basis for more elaborate requirements for more complicated detectors.
/*!
	The dataSource() method will play a very key role in some of the implementations of this class.
	It expects that dataSource() will return whichever is the expected final output spectrum the
	detector provides.

	It builds a control set where all controls should be kept.  After all controls have been constructed,
	the allControlsCreated() method should be called.  This will populate the list with all of the base
	class controls (this includes all of the spectrum and icr/ocr, should they exist).

	Controls that require implementation:

		- acquireTimeControl_
		- elapsedTimeControl_
		- acquisitionStatusControl_
		- acquireControl_
		- spectraControls_ (add all the raw spectrum controls to this list)

  */
class AMXRFDetector : public AMDetector
{
	Q_OBJECT

public:
	/// Constructor.
	AMXRFDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~AMXRFDetector();

	/// Returns the type of the detector
	virtual int type() { return -1; }
	/// Returns the number of elements in the detector.
	int elements() const { return rawSpectraSources_.size(); }
	/// Returns the current acquisition dwell time from the integration time control
	virtual double acquisitionTime() const;
	/// Returns the acquisition time tolerance.  Used for automatic setting that should be re-implemented if you have specific requirements.
	virtual double acquisitionTimeTolerance() const;
	/// Returns the elapsed time from the elapsed time control.
	virtual double elapsedTime() const;
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;
	/// Returns the total count (sum of all bins) as the single reading.  This sums all the values available in dataSource().  Subclasses can re-implement if they have more convenient methods for returning this value.
	virtual AMNumber singleReading() const;
	/// Re-implementing for optimizing the access speed.
	virtual bool reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;
	/// Returns whether this detector does dead time correction.
	virtual bool hasDeadTimeCorrection() const { return doDeadTimeCorrection_; }
	/// Returns the dead time as a percentage.  If there are multiple dead times (for many elements) then it returns the worst.
	virtual double deadTime() const;
	/// Returns a specific dead time as a percentage.
	virtual double deadTimeAt(int index) const;
	/// Returns whether the element is enabled or not.  Elements are zero indexed.
	bool isElementEnabled(int index) const;
	/// Returns true if the element at the given index is disabled. Elements are zero indexed.
	bool isElementDisabled(int index) const;
	/// Returns whether the element can be enabled. Elements are zero indexed.
	virtual bool canEnableElement(int index) const { Q_UNUSED(index) return true; }
	/// Returns whether the element can be disabled. Elements are zero indexed.
	virtual bool canDisableElement(int index) const { Q_UNUSED(index) return true; }

	// Returns the list of spectra controls.
	QList<AMReadOnlyPVControl*> spectraControls() const { return spectraControls_; }

	// The dead time data sources.  Dead time corrections are input/output and to get the percentage, 1 - output/input.
	/// Returns the input count data sources.
	QList<AMDataSource *> inputCountSources() const { return icrSources_; }
	/// Returns the input count data source at the given index.
	AMDataSource *inputCountSourceAt(int index) const { return (index >= 0 && index < icrSources_.count()) ? icrSources_.at(index) : 0; }
	/// Returns the output count data sources.
	QList<AMDataSource *> outputCountSources() const { return ocrSources_; }
	/// Returns the output count data source at the given index.
	AMDataSource *outputCountSourceAt(int index) const { return (index >= 0 && index < ocrSources_.count()) ? ocrSources_.at(index) : 0; }

	/// Returns the primary data source for viewing the detector's output.
	virtual AMDataSource *dataSource() const { return primarySpectrumDataSource_; }
	/// Returns all of the raw spectrum data sources.
	QList<AMDataSource *> rawSpectrumSources() const { return rawSpectraSources_; }
	/// Returns all of the analyzed spectrum data sources.
	QList<AMDataSource *> analyzedSpectrumSources() const { return analyzedSpectraSources_; }
	/// Returns all of the spectrum data sources, raw sources come first.
	QList<AMDataSource *> allSpectrumSources() const { return QList<AMDataSource *>() << rawSpectraSources_ << analyzedSpectraSources_; }

	/// Although common, not all implementations will have an elapsed time.  If your particular implementation doesn't then make sure this is false.
	virtual bool supportsElapsedTime() const = 0;

	/// Returns the number of regions of interest.
	int regionsOfInterestCount() const { return regionsOfInterest_.size(); }
	/// Returns the list of regions of interest that have been set on this detector.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }
	/// Returns the region of interest at a given index.
	AMRegionOfInterest *regionsOfInterestAt(int index) const { return regionsOfInterest_.at(index); }
	/// Returns the region of interest using the provided AMEmissionLine.
	AMRegionOfInterest *regionOfInterest(const AMEmissionLine &emissionLine) const;

	/// Returns a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual bool data(double *outputValues) const;

	/// Returns the ICR controls.
	QList<AMReadOnlyPVControl*> icrControls() const { return icrControls_; }
	/// Returns the ICR control at the given index, returns 0 if index is invalid.
	AMControl* icrControlAt(int index) const;

	/// Returns the acquire time control.
	AMControl* acquireTimeControl() const { return acquireTimeControl_; }

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// Adds a region of interest.  Does the work of creating the region and the data source associated with it.  Builds the region off of an AMEmissionLine.
	void addRegionOfInterest(const AMEmissionLine &emissionLine);
	/// Overloaded.  Adds a region of interest (make a copy of the regionOfInterest instance).  Does the work of creating the region and the data source associated with it.  The provided region is expected to be valid.
	void addRegionOfInterest(AMRegionOfInterest *regionOfInterest);
	/// Removes a region of interest.  Does the work of ensuring the region and the data source associated with it is properly removed.  Knows which region to remove based on the provided AMEmissionLine.
	void removeRegionOfInterest(const AMEmissionLine &emissionLine);
	/// Removes a region of interest.  Does the work of ensuring the region and the data source associated with it is properly removed.
	void removeRegionOfInterest(AMRegionOfInterest *regionOfInterest);
	/// Removes all regions of interest.
	void removeAllRegionsOfInterest();

	/// Enables the given element.  Elements are zero indexed.
	void enableElement(int elementId);
	/// Disables the given element.  Elements are zero indexed.
	void disableElement(int elementId);

signals:
	/// Notifier that the elapsed time has updated.
	void elapsedTimeChanged(double time);
	/// Notifier that a region of interest was added.  Passes the new region.
	void addedRegionOfInterest(AMRegionOfInterest *);
	/// Notifier that a region of interest was removed.  Passes the removed region.
	void removedRegionOfInterest(AMRegionOfInterest *);
	/// Notifier that the bounding range of a particular region of interest has changed.  Passes the region.
	void regionOfInterestBoundingRangeChanged(AMRegionOfInterest *);
	/// Notifier that the dead time has updated.
	void deadTimeChanged();
	/// Notifier that an element has been enabled.  Passes the element ID.
	void elementEnabled(int);
	/// Notifier that an element has been disabled. Passes the element ID.
	void elementDisabled(int);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	virtual void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out.
	virtual void onControlsTimedOut();
	/// Handles changes in the status control.
	virtual void onStatusControlChanged();
	/// Handles emitting the bounding range changed signal for regions of interest.
	void onRegionOfInterestBoundingRangeChanged(QObject *region);

protected:
	/// A helper method that adds all of this classes controls to the allControls_ control set.  This is required because we don't know in advance how many elements/spectra will be in the detector.  It also builds all of the spectra data sources and creates analysis blocks if necessary.
	void allControlsCreated();
	/// Basic initialization implementation for an XRF detector.  Subclass for more specific behaviour.
	virtual bool initializeImplementation();
	/// Basic initialization implementation for an XRF detector.  Subclass for more specific behaviour.
	virtual bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	/// Basic initialization implemenation for an XRF detector.  Subclass for more specific behaviour.
	virtual bool cleanupImplementation();
	/// This function is called from the Cancelling (acquisition) state for detectors that support cancelling acquisitions. Once the detector has successfully cancelled the acquisition you must call setAcquisitionCancelled()
	virtual bool cancelAcquisitionImplementation();
	/// This function is called if there is anything extra you need to do when adding regions of interest.  Default behaviour does nothing extra.
	virtual void addRegionOfInterestImplementation(AMRegionOfInterest *newRegionOfInterest);
	/// This function is callled if there is anythign extra you need to do when removing regions of interest.  Default behaviour does nothing extra.
	virtual void removeRegionOfInterestImplementation(AMRegionOfInterest *regionOfInterest);
	/// Helper function that updates the primary spectrum source to only use the enabled elements.
	void updatePrimarySpectrumSources();

	// Controls.  It is up to subclasses to ensure these are properly instantiated.
	/// Control handling the acquire time.
	AMControl *acquireTimeControl_;
	/// Control handling the elaspsed time.
	AMReadOnlyPVControl *elapsedTimeControl_;
	/// Control handling the acquisition state.
	AMReadOnlyPVControl *acquisitionStatusControl_;
	/// Control handling acquisition.
	AMPVControl *acquireControl_;
	/// List of all the spectrum controls.
	QList<AMReadOnlyPVControl *> spectraControls_;
	/// List of all ICR controls.
	QList<AMReadOnlyPVControl *> icrControls_;
	/// List of all OCR controls.
	QList<AMReadOnlyPVControl *> ocrControls_;

	/// The master set of controls
	AMControlSet *allControls_;

	// Spectrum related data sources.
	/// List of all the spectrum data sources.
	QList<AMDataSource *> rawSpectraSources_;
	/// List of all the ICR data sources.
	QList<AMDataSource *> icrSources_;
	/// List of all the OCR data sources.
	QList<AMDataSource *> ocrSources_;
	/// List of all analyzed data sources.
	QList<AMDataSource *> analyzedSpectraSources_;
	/// The primary spectrum data source.  This is the data source that will be returned by AMDataSource::dataSource().
	AMDataSource *primarySpectrumDataSource_;

	/// The list of enabled elements.  Values are 0 indexed.
	QList<int> enabledElements_;

	// Regions of interest and their data sources.
	/// List of all the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
	/// The signal mapper that helps manage the signals from the regions of interest.
	QSignalMapper *regionOfInterestSignalMapper_;

	// Extras
	/// Flag that holds whether the detector will do dead time corrections or not.
	bool doDeadTimeCorrection_;

private:
	/// Internal method that builds process variable data sources for the spectrum controls.
	void buildSpectrumDataSources();
	/// Internal method that builds the process variable data sources for the ICR and OCR controls.
	void buildDeadTimeDataSources();
	/// Internal method that builds all the analysis blocks.  Includes dead time correction and summing analysis blocks.  Sets the primarySpectrumDataSource_.
	void buildAllAnalysisBlocks();
};

#endif // AMXRFDETECTOR_H
