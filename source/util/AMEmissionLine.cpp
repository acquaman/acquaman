#include "AMEmissionLine.h"

#include <QStringList>
#include <QStringBuilder>

 AMEmissionLine::~AMEmissionLine(){}
AMEmissionLine::AMEmissionLine(const QString &name, double energy)
{
	name_ = name;
	energy_ = energy;

	QStringList symbolAndLine = name.split(" ");

	// The expected form of the name is "Symbol Line".  If not in this form, the symbol and emission line are empty.
	if (symbolAndLine.size() == 2){

		symbol_ = symbolAndLine.first();
		lineName_ = symbolAndLine.last();
		greekLineName_ = symbolAndLine.last().replace("a", QString::fromUtf8("α"));
		greekLineName_ = symbolAndLine.last().replace("b", QString::fromUtf8("β"));
		greekLineName_ = symbolAndLine.last().replace("g", QString::fromUtf8("γ"));
		greekName_ = symbol_ % " " % greekLineName_;
	}

	else{

		symbol_ = "";
		lineName_ = "";
		greekLineName_ = "";
		greekName_ = "";
	}
}

AMEmissionLine::AMEmissionLine(const QString &name, const QString &energy)
{
	name_ = name;
	energy_ = energy.toDouble();

	QStringList symbolAndLine = name.split(" ");

	// The expected form of the name is "Symbol Line".  If not in this form, the symbol and emission line are empty.
	if (symbolAndLine.size() == 2){

		symbol_ = symbolAndLine.first();
		lineName_ = symbolAndLine.last();
		greekLineName_ = symbolAndLine.last().replace("a", QString::fromUtf8("α"));
		greekLineName_ = symbolAndLine.last().replace("b", QString::fromUtf8("β"));
		greekLineName_ = symbolAndLine.last().replace("g", QString::fromUtf8("γ"));
		greekName_ = symbol_ % " " % greekLineName_;
	}

	else{

		symbol_ = "";
		lineName_ = "";
		greekLineName_ = "";
		greekName_ = "";
	}
}

AMEmissionLine::AMEmissionLine(const AMEmissionLine &original)
{
	name_ = original.name();
	energy_ = original.energy();
	symbol_ = original.elementSymbol();
	lineName_ = original.lineName();
	greekName_ = original.greekName();
	greekLineName_ = original.greekLineName();
}

AMEmissionLine::AMEmissionLine()
{
	name_ = QString();
	energy_ = -1;
	symbol_ = QString();
	lineName_ = QString();
	greekName_ = QString();
	greekLineName_ = QString();
}

AMEmissionLine &AMEmissionLine::operator =(const AMEmissionLine &other)
{
	if (this != &other){

		name_ = other.name();
		energy_ = other.energy();
		symbol_ = other.elementSymbol();
		lineName_ = other.lineName();
		greekName_ = other.greekName();
		greekLineName_ = other.greekLineName();
	}

	return *this;
}

bool AMEmissionLine::operator ==(const AMEmissionLine &other) const
{
	return (name_ == other.name())
			&& (energy_ == other.energy())
			&& (symbol_ == other.elementSymbol())
			&& (lineName_ == other.lineName())
			&& (greekName_ == other.greekName())
			&& (greekLineName_ == other.greekLineName());
}

bool AMEmissionLine::operator !=(const AMEmissionLine &other) const
{
	return !(this->operator ==(other));
}

bool AMEmissionLine::operator <(const AMEmissionLine &other) const
{
	return energy_ < other.energy();
}

bool AMEmissionLine::operator <=(const AMEmissionLine &other) const
{
	return energy_ <= other.energy();
}

bool AMEmissionLine::operator >(const AMEmissionLine &other) const
{
	return energy_ > other.energy();
}

bool AMEmissionLine::operator >=(const AMEmissionLine &other) const
{
	return energy_ >= other.energy();
}
