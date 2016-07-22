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


#ifndef CLSPGTDETECTORV2_H
#define CLSPGTDETECTORV2_H

#include "beamline/AMDetector.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class CLSPGTDetectorV2 : public AMDetector
{
Q_OBJECT
public:
	/// Default constructor. Requires the name and base PV of the detector. It builds all the PV's and connects them accordingly.
	CLSPGTDetectorV2(const QString &name, const QString &description, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSPGTDetectorV2();

	/// Returns the number of dimensions in the output of this detector. This is a spectrum detector, so it has a rank of 1.
	virtual int rank() const { return 1; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.  Currently this is hardcoded to 1024.
	virtual AMnDIndex size() const { return AMnDIndex(1024); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const;

	/// The PGT explicitly require powering on
	virtual bool requiresPower() const { return true; }

	/// Cancelling is not implemented for the PGT detector
	virtual bool canCancel() const { return false; }
	/// Clearing is currently supported for the PGT detector
	virtual bool canClear() const { return true; }

	/// PGT is not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the current acquisition dwell time from the integration time control
	virtual double acquisitionTime() const;
	/// Returns the acquisition time tolerance.
	virtual double acquisitionTimeTolerance() const;
	/// The PGT can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// The PGT shares a triggering source sometimes uses the synchronized dwell time object
	virtual bool sharesDetectorTriggerSource() const;
	/// Returns the synchronized dwell time trigger source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource();
	/// Returns the synchronized dwell time dwell time source if we're currently enabled, otherwise a null pointer
	virtual AMDetectorDwellTimeSource* detectorDwellTimeSource();

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the total count (sum of all bins) as the single reading
	virtual AMNumber singleReading() const;

	/// Fills a (hopefully) valid pointer to a block of detector data in row-major order (first axis varies slowest)
	virtual bool data(double *outputValues) const;

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return spectrumDataSource_; }

	/// Returns the integration mode control if the caller is of a privileged type (CLSPGTDetectorV2View)
	AMControl* privilegedIntegrationModeControl(const QObject *caller);

public slots:
	/// Set the acquisition dwell time for triggered (RequestRead) detectors
	virtual bool setAcquisitionTime(double seconds);

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// PGT detector supports clearing
	virtual bool clear();

signals:
	void acquisitionModeChanged();

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();

	/// Handles changes in the status control
	void onStatusControlChanged(double value);

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cleanupImplementation();

protected:
	/// The status control
	AMControl *statusControl_;
	/// The integration time control
	AMControl *integrationTimeControl_;
	/// The integration mode contorl
	AMControl *integrationModeControl_;
	/// The detector start control
	AMControl *startAcquisitionControl_;
	/// The detector spectrum control
	AMControl *spectrumControl_;
	/// A binned version of the detector spectrum control
	AMControl *binnedSpectrumControl_;
	/// The clear control
	AMControl *clearControl_;
	/// The power on/off control
	AMControl *powerControl_;
	/// The voltage control
	AMControl *voltageControl_;

	/// The master set of controls
	AMControlSet *allControls_;

	/// The list of all the raw spectrum data sources.
	AM1DProcessVariableDataSource *spectrumDataSource_;

	/// PV basename for the detector instance
	QString baseName_;
};

#endif // CLSPGTDETECTORV2_H
