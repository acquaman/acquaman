#ifndef GENERALUTILITIES_H
#define GENERALUTILITIES_H

#include <QString>

class GeneralUtilities
{
public:

	/// Takes a emission line name and turns the greek letter into a roman equivalent and returns the result (ie: alpha -> a).
	static QString removeGreek(QString name);
	/// Takes a ASCII plain text emission line name and adds the corresponding greek letter to it (ie: Ka1 -> Kalpha1).
	static QString addGreek(QString name);
};

#endif // GENERALUTILITIES_H
