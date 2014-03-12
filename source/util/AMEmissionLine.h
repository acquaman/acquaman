#ifndef AMEMISSIONLINE_H
#define AMEMISSIONLINE_H

#include <QString>

/// Basic encapsulation of an emission line.
class AMEmissionLine
{
public:
	/// Builds an emission line from the name and energy provided.  The name is expected to be in the form "Fe Ka1" with no greek letters.
 	virtual ~AMEmissionLine();
	AMEmissionLine(const QString &name, double energy);
	/// Builds an emission line from the name and  The name is expected to be in the form "Fe Ka1" with no greek letters.
	AMEmissionLine(const QString &name, const QString &energy);
	/// Copy constructor.
	AMEmissionLine(const AMEmissionLine &original);
	/// Null constructor.  Creates a null AMAbsorptionEdge.
	AMEmissionLine();

	/// Returns the name of the emission line.
	QString name() const { return name_; }
	/// Returns the energy of the emission line as a double.
	double energy() const { return energy_; }
	/// Returns the energy of the emission line as a string.
        QString energyString(char format = 'f', int precision = 2) const { return QString::number(energy_, format, precision); }
	/// Returns the symbol for the element associated with this emission line.
	QString elementSymbol() const { return symbol_; }
	/// Returns the emission line name for the emission line.
	QString lineName() const { return lineName_; }
	/// Returns the full name but with the greek line name.
	QString greekName() const { return greekName_; }
	/// Returns the line name but with greek characters.
	QString greekLineName() const { return greekLineName_; }
	/// Returns whether the emission line is null or not.
	bool isNull() const { return name_.isNull(); }

	/// The equals operator.  Sets the name and energy to the same as the provided AMEmissionLine.
	AMEmissionLine &operator =(const AMEmissionLine &other);
	/// The equivalent operator.  Checks the name and energy of the provided AMEmissionLine.
	bool operator ==(const AMEmissionLine &other) const;
	/// The not-equivalent operator.  Logical not of the equivalent operator.
	bool operator !=(const AMEmissionLine &other) const;
	/// The less-than operator.  Returns true if energy of this emission line is less than other's.
	bool operator <(const AMEmissionLine &other) const;
	/// The greater-than operator.  Returns true if the energy of this emission line is greater than other's.
	bool operator >(const AMEmissionLine &other) const;
	/// The less-than-or-equal-to operator.  Returns true if the energy of this emission line is less than or equal to other's.
	bool operator <=(const AMEmissionLine &other) const;
	/// The greater-than-or-equal-to operator.  Returns true if the energy of this emission line is greater than or equal to other's.
	bool operator >=(const AMEmissionLine &other) const;

protected:
	/// The full name of the emission line.
	QString name_;
	/// The energy of the emission line.
	double energy_;
	/// The element symbol for this particular emission line.
	QString symbol_;
	/// The emission line name.
	QString lineName_;
	/// The full name with greek characters.
	QString greekName_;
	/// The emission line name with greek characters.
	QString greekLineName_;
};

#endif // AMEMISSIONLINE_H
