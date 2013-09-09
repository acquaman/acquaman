#ifndef AMEMISSIONLINE_H
#define AMEMISSIONLINE_H

#include <QString>

/// Basic encapsulation of an emission line.
class AMEmissionLine
{
public:
	/// Builds an emission line from the name and energy provided.
	AMEmissionLine(const QString &name, double energy);
	/// Builds an emission line from the name and

protected:
	/// The name of the edge.
	QString name_;
	/// The energy of the edge.
	double energy_;
};

#endif // AMEMISSIONLINE_H
