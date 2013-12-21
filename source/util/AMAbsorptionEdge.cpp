#include "AMAbsorptionEdge.h"

#include <QStringList>

 AMAbsorptionEdge::~AMAbsorptionEdge(){}
AMAbsorptionEdge::AMAbsorptionEdge(const QString &name, double energy)
{
	name_ = name;
	energy_ = energy;

	QStringList symbolAndEdge = name.split(" ");

	// The expected form of the name is "Symbol Edge".  If not in this form, the symbol and edge are empty.
	if (symbolAndEdge.size() == 2){

		symbol_ = symbolAndEdge.first();
		edge_ = symbolAndEdge.last();
	}

	else{

		symbol_ = "";
		edge_ = "";
	}
}

AMAbsorptionEdge::AMAbsorptionEdge(const QString &name, const QString &energy)
{
	name_ = name;
	energy_ = energy.toDouble();

	QStringList symbolAndEdge = name.split(" ");

	// The expected form of the name is "Symbol Edge".  If not in this form, the symbol and edge are empty.
	if (symbolAndEdge.size() == 2){

		symbol_ = symbolAndEdge.first();
		edge_ = symbolAndEdge.last();
	}

	else{

		symbol_ = "";
		edge_ = "";
	}
}

AMAbsorptionEdge::AMAbsorptionEdge(const AMAbsorptionEdge &original)
{
	name_ = original.name();
	energy_ = original.energy();
	symbol_ = original.elementSymbol();
	edge_ = original.edgeName();
}

AMAbsorptionEdge::AMAbsorptionEdge()
{
	name_ = QString();
	energy_ = -1;
	symbol_ = QString();
	edge_ = QString();
}

AMAbsorptionEdge &AMAbsorptionEdge::operator =(const AMAbsorptionEdge &other)
{
	if (this != &other){

		name_ = other.name();
		energy_ = other.energy();
		symbol_ = other.elementSymbol();
		edge_ = other.edgeName();
	}

	return *this;
}

bool AMAbsorptionEdge::operator ==(const AMAbsorptionEdge &other) const
{
	return (name_ == other.name())
			&& (energy_ == other.energy())
			&& (symbol_ == other.elementSymbol())
			&& (edge_ == other.edgeName());
}

bool AMAbsorptionEdge::operator !=(const AMAbsorptionEdge &other) const
{
	return !(this->operator ==(other));
}

bool AMAbsorptionEdge::operator <(const AMAbsorptionEdge &other) const
{
	return energy_ < other.energy();
}

bool AMAbsorptionEdge::operator <=(const AMAbsorptionEdge &other) const
{
	return energy_ <= other.energy();
}

bool AMAbsorptionEdge::operator >(const AMAbsorptionEdge &other) const
{
	return energy_ > other.energy();
}

bool AMAbsorptionEdge::operator >=(const AMAbsorptionEdge &other) const
{
	return energy_ >= other.energy();
}
