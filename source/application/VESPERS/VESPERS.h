#ifndef VESPERS_H
#define VESPERS_H

/*!
	This namespace contains enums that are used throughout the VESPERS application.  It is meant to consolidate them all
	in one location to minimize duplicated code.  Some of these enums are declared many times throughout the program
	and various classes.
  */


namespace VESPERS {

	/// Enum for the different beams.
	/*!
	 - Pink is not monochromatized and contains all the energies from the bending magnet.
	 - TenPercent (10%) is a broad band pass filter.
	 - OnePointSixPercent (1.6%) is a narrow band pass filter.
	 - Si is the monochromator with 0.01% band pass.
	  */
	enum Beam { NoBeam = 0, Pink, TenPercent, OnePointSixPercent, Si };

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { NoXRF = 0, SingleElement = 1, FourElement = 2 };

	/// Enum for the ion chambers used in scans.  These are used for the incoming or transmitted total intensity.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };

	/// Enum for choosing the geometry.
	/*!
		The following are the available choices for the geometry.  Note that the Big beam option is technically available, but most of the
		hardware for that part of the endstation is not available currently.  What will be used there is still the same though.

		- Invalid:	Used as the null or uninitialised value.
		- StraightOn:  This has the sample sitting perpendicular to the beam.  When using the sample stage, it uses the x and z motors and
					only has XAS and XRF mapping available.  The current setup has ion chambers and the single element vortex detector.
		- Single45Vertical:	This has the sample sitting at 45 degrees vertically such that reflection based diffraction patterns can be
					measured as well as any XRF or XAS measurements.  When using the sample stage, it uses the pseudo-motors H and V and
					has all of the techniques available to it (XAS, XRF, XRD).  The current setup has ion chambers, single element vortex
					detector, and the Roper CCD.
		- Single45Horizontal:  This has the sample sitting at 45 degrees horizontally.  When using the sample stage, it uses the x and z
					motors and only has XAS and XRF mapping available.  The current setup has ion chambers and the four element vortex
					detector.
		- Double45:	This has the sample sitting at 45 degrees both vertically and horizontally such that reflection based diffraction
					patterns can be measured as well as XRF or XAS measurements.  When using the sample stage, it uses the pseudo-motors
					H and V and has all of the techniques available to it (XAS, XRF, XRD).  The current setup has ion chambers, four element
					vortex detector, and the Roper CCD.
		- BigBeam:	This has the sample sitting upstream of the Pre-KB ion chamber.  This setup has a completely different sample stage
					and the only techniques available to it are XAS and XRF mapping (macro-probe mapping).  The current setup has ion chambers
					and the four element vortex detector.
	  */
	enum Geometry { Invalid = 0, StraightOn, Single45Vertical, Single45Horizontal, Double45, BigBeam };
}

#endif // VESPERS_H
