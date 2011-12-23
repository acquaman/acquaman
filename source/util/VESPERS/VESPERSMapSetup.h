/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSMAPSETUP_H
#define VESPERSMAPSETUP_H

#include <QObject>

class VESPERSMapSetup : public QObject
{
	Q_OBJECT
public:
	explicit VESPERSMapSetup(QObject *parent = 0);

	/// Returns whether the configuration is a 2D map.
	bool is2D() const { return is2D_; }
	/// Returns whether the configuration is a 3D map.
	bool is3D() const { return !is2D_; }
	/// Returns whether the configuration is using the single element vortex detector.
	bool usingSingleElement() const { return usingSingleEl_; }
	/// Returns whether the configuration is using the four element vortex detector.
	bool usingFourElement() const { return !usingSingleEl_; }
	/// Returns whether the CCD is enabled.
	bool usingCCD() const { return usingCCD_; }
	/// Returns whether the configuration is putting multiple images inside a single file.
	bool usingMultipleImages() const { return usingMultipleImages_; }

signals:
	/// Notifier that the dimensionality has changed.  True = 2D, false = 3D.
	void dimensionChanged(bool);
	/// Notifier that the detector has changed.  True = single element, false = four element.
	void detectorChanged(bool);
	/// Notifier whether the CCD is being used.
	void ccdBeingUsed(bool);
	/// Notifier whether images will be stored in a single file.
	void multipleImagesUsed(bool);

public slots:
	/// Sets the configuration to be a 2D map.
	void set2D() { if (!is2D_) emit dimensionChanged(is2D_ = true); }
	/// Sets the configuration to be a 3D map.
	void set3D() { if (is2D_) emit dimensionChanged(is2D_ = false); }
	/// Sets the configuration to use the single element vortex detector.
	void useSingleElement() { if (!usingSingleEl_) emit detectorChanged(usingSingleEl_ = true); }
	/// Sets the configuration to use the four element vortex detector.
	void useFourElement() { if (usingSingleEl_) emit detectorChanged(usingSingleEl_ = false); }
	/// Sets the configuration to use the CCD detector based on \param use.
	void useCCD(bool use) { if (usingCCD_ != use) emit ccdBeingUsed(usingCCD_ = use); }
	/// Sets whether the CCD images get grouped into a single file (multiple images per file, max 50).  Does nothing if the CCD is not enabled.
	void useMultiImages(bool use) { if (usingCCD_ && (usingMultipleImages_ != use)) emit multipleImagesUsed(usingMultipleImages_ = use); }

	/// Handles launching the program with the given arguments.
	void launchNDMapper();

protected:
	/// Flag holding whether the map will be either 2D or 3D.  False implies 3D.
	bool is2D_;
	/// Flag holding whether the map will be using either the single element vortex or four element vortex.  False implies using the four element.
	bool usingSingleEl_;
	/// Flag holding whether the CCD is enabled.
	bool usingCCD_;
	/// Flag holding whether multiple images will be stored in a single file (max 50).
	bool usingMultipleImages_;
};

#endif // VESPERSMAPSETUP_H
