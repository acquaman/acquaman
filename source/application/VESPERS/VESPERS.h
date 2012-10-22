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
	enum Beam { None = 0, Pink, TenPercent, OnePointSixPercent, Si };

	/// Enum for the ion chambers used in scans.  These are used for the incoming or transmitted total intensity.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };
}

#endif // VESPERS_H
