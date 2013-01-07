/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSCONFIGURATIONFILEBUILDER_H
#define VESPERSCONFIGURATIONFILEBUILDER_H

#include <QObject>

/*!
	This class builds a DACQ configuration file based on standard settings for detectors and scan types.
	Currently, the VESPERS beamline has the following detectors:

		- Ion chambers
		- Single element vortex
		- Four element vortex
		- Roper CCD

	The scan types can be enery scans or spatial scans (1D or 2D)
  */
class VESPERSConfigurationFileBuilder : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit VESPERSConfigurationFileBuilder(QObject *parent = 0);

	/// Based on the flags set, builds a configuration file.  The result is saved in the provided directory as template.cfg.
	bool buildConfigurationFile();

	/// Sets the number of dimensions.
	void setDimensions(int dim) { dimensions_ = dim; }
	/// Sets whether the single element should be included.
	void setSingleElement(bool use) { singleElement_ = use; }
	/// Sets whether the four element should be included.
	void setFourElement(bool use) { fourElement_ = use; }
	/// Sets whether the Roper CCD should be included.
	void setRoperCCD(bool use) { roperCCD_ = use; }
	/// Sets whether the Mar CCD should be included.
	void setMarCCD(bool use) { marCCD_ = use; }
	/// Sets the first scan axis PV name.
	void setPvNameAxis1(const QString &name) { pvNameAxis1_ = name; }
	/// Sets the second scan axis PV name.
	void setPvNameAxis2(const QString &name) { pvNameAxis2_ = name; }

	/// Returns the number of dimensions.
	int dimensions() const { return dimensions_; }
	/// Returns whether the single element is included.
	bool singleElement() const { return singleElement_; }
	/// Returns whether the four element is included.
	bool fourElement() const { return fourElement_; }
	/// Returns whether the Roper CCD is included.
	bool roperCCD() const { return roperCCD_; }
	/// Returns whetehr the Mar CCD is included.
	bool marCCD() const { return marCCD_; }
	/// Returns the first scan axis PV name.
	QString pvNameAxis1() const { return pvNameAxis1_; }
	/// Returns the second scan axis PV name.
	QString pvNameAxis2() const { return pvNameAxis2_; }

signals:

public slots:

protected:

	/// The number of dimensions.
	int dimensions_;
	/// Flag whether or not the single element detector is being used.
	bool singleElement_;
	/// Flag whether or not the four element detector is being used.
	bool fourElement_;
	/// Flag whether or not the Roper CCD is being used.
	bool roperCCD_;
	/// Flag whether or not the Mar CCD is being used.
	bool marCCD_;
	/// String holding the first (or only) scan axis PV name.
	QString pvNameAxis1_;
	/// String holding the second (if applicable) scan axis PV name.
	QString pvNameAxis2_;
};

#endif // VESPERSCONFIGURATIONFILEBUILDER_H
