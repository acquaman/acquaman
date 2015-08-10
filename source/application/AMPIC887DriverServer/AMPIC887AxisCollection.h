#ifndef AMPIC887AXISCOLLECTION_H
#define AMPIC887AXISCOLLECTION_H
#include <QList>
#include "AMGCS2.h"

/*!
  * A class representing a collection of axes controlled by a PI C887.11 Controller
  */
class AMPIC887AxisCollection : public QList<AMGCS2::Axis>
{
public:
	// Enumerates the different states the collection can be initialized in.
	enum InitializationState {
		EmptyCollection,	// Collection is initialized with no contained axes
		AllAxes,			// Collection is initialized with all axes
		LinearAxes,			// Collection is initialized with linear axes (X, Y, Z)
		RotationalAxes		// Collection is initialized with rotational axes (U, V, W)
	};

	// Enumerates the different valid states which the colleciton can be in
	enum ValidState {
		Valid,					// Collection is valid
		ContainsUnknownAxis,	// Collection contains an unknown axis
		ContainsDuplicateAxes,	// Collection contains duplicates
		UnknownValidState		// Collections valid state is unknown
	};

	/*!
	  * Creates an instance of an axis collection initialized depending on the
	  * provided initialization state.
	  * \param initializationState ~ An optional parameter which indicates the
	  * data which should be contained in the created collection. If none is
	  * provided the collection is initialized with all axes.
	  */
	AMPIC887AxisCollection(InitializationState initializationState = AllAxes);

	/*!
	  * Creates an instance of an axis collection which contains the provided
	  * list of axes.
	  * \param axes ~ The list of axes from which to build the axis collection.
	  */
	explicit AMPIC887AxisCollection(const QList<AMGCS2::Axis>& axes);

	/*!
	  * The valid state of the axis collection.
	  * \returns ValidState indicating the validity of the collection.
	  */
	ValidState validate() const;

	/*!
	  * Whether this collection contains any of the rotational (U, V, W) axes.
	  */
	bool containsRotationalAxes() const;

	/*!
	  * A string containing the axes of the collection in the form expected by
	  * the GCS2 command system.
	  */
	QString toString() const;

	/*!
	  * Static helper method which attempts to parse the provided string in order
	  * to create an axis collection. The provided string can be in one of two valid
	  * forms:
	  *  - Empty: In which case the collection returned will be determined by
	  *    the stateIfEmpty parameter.
	  *  - Contain single characters delimited by a space: In which case the collection
	  *    will be returned containing axes which match these letters.
	  *
	  * NOTE: In the second case no check is made to ensure that the single characters
	  * are known axes, or if the provided string contains duplicates. As such a
	  * validate() check should be made on the returned collection.
	  * \param axesString ~ The string to build the collection from.
	  * \param parseSuccess ~ Optional out parameter which will be set to true
	  * if the parse was successful, false otherwise. If none is provided no aciton
	  * will be taken to indicate success.
	  * \param stateIfEmpty ~ Optional parameter which will determine what the
	  * collection should contain in the event that the provided string is empty.
	  * If none is set the collection will be returned with all axes.
	  */
	static AMPIC887AxisCollection fromString(const QString& axesString,
											 bool* parseSuccess = 0,
											 InitializationState stateIfEmpty = AllAxes);
};

#endif // AMPIC887AXISCOLLECTION_H
