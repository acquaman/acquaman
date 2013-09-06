#ifndef AMXRFDETECTOR_H
#define AMXRFDETECTOR_H

#include "beamline/AMDetector.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

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

	/// Returns the number of elements in the detector.
	int elements() const { return rawSpectraSources_.size(); }
	/// Returns the current acquisition dwell time from the integration time control
	virtual double acquisitionTime() const;
	/// Returns the elapsed time from the elapsed time control.
	virtual double elapsedTime() const;
	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;
	/// Returns the total count (sum of all bins) as the single reading.  This sums all the values available in dataSource().  Subclasses can re-implement if they have more convenient methods for returning this value.
	virtual AMNumber singleReading() const;
	/// Re-implementing for optimizing the access speed.
	virtual bool reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const;

	/// Returns the primary data source for viewing the detector's output.
	virtual AMDataSource *dataSource() const { return primarySpectrumDataSource_; }
	/// Returns all of the raw spectrum data sources.
	QList<AMDataSource *> rawSpectrumSources() const { return rawSpectraSources_; }
	/// Returns all of the analyzed spectrum data sources.
	QList<AMDataSource *> analyzedSpectrumSources() const { return analyzedSpectraSources_; }
	/// Returns all of the spectrum data sources, raw sources come first.
	QList<AMDataSource *> allSpectrumSources() const { return QList<AMDataSource *>() << rawSpectraSources_ << analyzedSpectraSources_; }


public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	virtual void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out.
	virtual void onControlsTimedOut();
	/// Handles changes in the status control.
	void onStatusControlChanged();

protected:
	/// A helper method that adds all of this classes controls to the allControls_ control set.  This is required because we don't know in advance how many elements/spectra will be in the detector.  It also builds all of the spectra data sources and creates analysis blocks if necessary.
	void allControlsCreated();
	/// Basic initialization implementation for an XRF detector.  Subclass for more specific behaviour.
	bool initializeImplementation();
	/// Basic initialization implementation for an XRF detector.  Subclass for more specific behaviour.
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	/// Basic initialization implemenation for an XRF detector.  Subclass for more specific behaviour.
	bool cleanupImplementation();

	// Controls.  It is up to subclasses to ensure these are properly instantiated.
	/// Control handling the acquire time.
	AMPVControl *acquireTimeControl_;
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

	// Data sources.
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
