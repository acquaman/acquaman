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


#ifndef ELEMENT_H
#define ELEMENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>

#include "util/AMAbsorptionEdge.h"
#include "util/AMEmissionLine.h"

/*! The Element class is meant as a consolidated container for all the information of an element.  You do not need to provide the edges or emission lines if they are not known.
  If the particular element does not have the required attribute then pass in an empty string.  For simplicity, many of the items come as QPairs.
  This is particularly noticeable with absorption edges.  They come as a QPair< edge name, energy >.  This way you will always know what this particular
  energy is associated with what edge.
  */
class AMElement : public QObject
{
	Q_OBJECT

public:

	/// Constructor for the element.  For the edgeList and emissionLineList, the edges and lines energy values \em must be in order (ie: edges { K, L1, L2, L3, ... }, lines { Ka1, Ka2, ... }.
	explicit AMElement(QString name, QString symbol, QString atomicNumber, QStringList edgeList, QStringList emissionLineList, QObject *parent = 0);

	/// Returns the name of the element.
	QString name() const { return name_; }
	/// Returns the symbol of the element.
	QString symbol() const { return symbol_; }
	/// Returns the atomic number.
	int atomicNumber() const { return atomicNumber_; }
	/// Returns the atomic number as a string
	QString atomicNumberString() const { return QString("%1").arg(atomicNumber_); }
	/// Returns the entire list of edges.
	QList<AMAbsorptionEdge> edges() const { return edges_; }
	/// Returns the entire list of emission lines.
	QList<QPair<QString, QString> > emissionLines() const { return emissionLines_; }

	// Helper functions that give access directly to a particular edge.
	/// Returns the K-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge KEdge() const { return (0 < edgeSize_) ? edges_.at(0) : AMAbsorptionEdge(); }
	/// Returns the L1-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge L1Edge() const { return (1 < edgeSize_) ? edges_.at(1) : AMAbsorptionEdge(); }
	/// Returns the L2-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge L2Edge() const { return (2 < edgeSize_) ? edges_.at(2) : AMAbsorptionEdge(); }
	/// Returns the L3-edge.  RReturns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge L3Edge() const { return (3 < edgeSize_) ? edges_.at(3) : AMAbsorptionEdge(); }
	/// Returns the M1-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge M1Edge() const { return (4 < edgeSize_) ? edges_.at(4) : AMAbsorptionEdge(); }
	/// Returns the M2-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge M2Edge() const { return (5 < edgeSize_) ? edges_.at(5) : AMAbsorptionEdge(); }
	/// Returns the M3-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge M3Edge() const { return (6 < edgeSize_) ? edges_.at(6) : AMAbsorptionEdge(); }
	/// Returns the M4-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge M4Edge() const { return (7 < edgeSize_) ? edges_.at(7) : AMAbsorptionEdge(); }
	/// Returns the M5-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge M5Edge() const { return (8 < edgeSize_) ? edges_.at(8) : AMAbsorptionEdge(); }
	/// Returns the N1-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N1Edge() const { return (9 < edgeSize_) ? edges_.at(9) : AMAbsorptionEdge(); }
	/// Returns the N2-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N2Edge() const { return (10 < edgeSize_) ? edges_.at(10) : AMAbsorptionEdge(); }
	/// Returns the N3-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N3Edge() const { return (11 < edgeSize_) ? edges_.at(11) : AMAbsorptionEdge(); }
	/// Returns the N4-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N4Edge() const { return (12 < edgeSize_) ? edges_.at(12) : AMAbsorptionEdge(); }
	/// Returns the N5-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N5Edge() const { return (13 < edgeSize_) ? edges_.at(13) : AMAbsorptionEdge(); }
	/// Returns the N6-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N6Edge() const { return (14 < edgeSize_) ? edges_.at(14) : AMAbsorptionEdge(); }
	/// Returns the N7-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge N7Edge() const { return (15 < edgeSize_) ? edges_.at(15) : AMAbsorptionEdge(); }
	/// Returns the O1-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge O1Edge() const { return (16 < edgeSize_) ? edges_.at(16) : AMAbsorptionEdge(); }
	/// Returns the O2-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge O2Edge() const { return (17 < edgeSize_) ? edges_.at(17) : AMAbsorptionEdge(); }
	/// Returns the O3-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge O3Edge() const { return (18 < edgeSize_) ? edges_.at(18) : AMAbsorptionEdge(); }
	/// Returns the O4-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge O4Edge() const { return (19 < edgeSize_) ? edges_.at(19) : AMAbsorptionEdge(); }
	/// Returns the O5-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge O5Edge() const { return (20 < edgeSize_) ? edges_.at(20) : AMAbsorptionEdge(); }
	/// Returns the P1-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge P1Edge() const { return (21 < edgeSize_) ? edges_.at(21) : AMAbsorptionEdge(); }
	/// Returns the P2-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge P2Edge() const { return (22 < edgeSize_) ? edges_.at(22) : AMAbsorptionEdge(); }
	/// Returns the P3-edge.  Returns a pair of null strings if the edge doesn't exist.
	AMAbsorptionEdge P3Edge() const { return (23 < edgeSize_) ? edges_.at(23) : AMAbsorptionEdge(); }

	// Helper functions that give access directly to a particular edge.
	/// Returns the K-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Kalpha1() const { if (0 < emissionLineSize_) return emissionLines_.at(0);  return qMakePair(QString(), QString()); }
	/// Returns the K-alpha2 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Kalpha2() const { if (1 < emissionLineSize_) return emissionLines_.at(1);  return qMakePair(QString(), QString()); }
	/// Returns the K-beta1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Kbeta1() const { if (2 < emissionLineSize_) return emissionLines_.at(2);  return qMakePair(QString(), QString()); }
	/// Returns the L-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lalpha1() const { if (3 < emissionLineSize_) return emissionLines_.at(3);  return qMakePair(QString(), QString()); }
	/// Returns the L-alpha2 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lalpha2() const { if (4 < emissionLineSize_) return emissionLines_.at(4);  return qMakePair(QString(), QString()); }
	/// Returns the L-beta1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lbeta1() const { if (5 < emissionLineSize_) return emissionLines_.at(5);  return qMakePair(QString(), QString()); }
	/// Returns the L-beta2 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lbeta2() const { if (6 < emissionLineSize_) return emissionLines_.at(6);  return qMakePair(QString(), QString()); }
	/// Returns the L-gamma1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lgamma1() const { if (7 < emissionLineSize_) return emissionLines_.at(7);  return qMakePair(QString(), QString()); }
	/// Returns the M-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Malpha1() const { if (8 < emissionLineSize_) return emissionLines_.at(8);  return qMakePair(QString(), QString()); }
	/// Returns the K-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Kalpha() const { return Kalpha1(); }
	/// Returns the K-beta1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Kbeta() const { return Kbeta1(); }
	/// Returns the L-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lalpha() const { return Lalpha1(); }
	/// Returns the L-beta1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lbeta() const { return Lbeta1(); }
	/// Returns the L-gamma1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Lgamma() const { return Lgamma1(); }
	/// Returns the M-alpha1 emission line.  Returns a pair of null strings if the emission line doesn't exist.
	QPair<QString, QString> Malpha() const { return Malpha1(); }

signals:

public slots:

private:

	/// The name of the element.
	QString name_;
	/// The elemental symbol for this particular element.
	QString symbol_;
	/// The atomic number of the element, stored as an int.
	int atomicNumber_;
	/// A list of all of the edges.  Pairs of <Name, Energy>.
	QList<AMAbsorptionEdge> edges_;
	/// A list of all of the emission lines.  Pairs of <Name, Energy>.
	QList<QPair<QString, QString> > emissionLines_;

	/// Cached sizes of the edge list for quick access.
	int edgeSize_;
	/// Cached sizes of the emission line list for quick access.
	int emissionLineSize_;
};

#endif // ELEMENT_H
