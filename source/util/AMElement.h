#ifndef ELEMENT_H
#define ELEMENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPair>

class AMElement : public QObject
{
	/*! The Element class is meant as a consolidated container for all the information of an element.  You do not need to provide the edges or emission lines if they are not known.
	  If the particular element does not have the required attribute then pass in an empty string.  For simplicity, many of the items come as QPairs.
	  This is particularly noticeable with absorption edges.  They come as a QPair< edge name, energy >.  This way you will always know what this particular
	  energy is associated with what edge.
	  */

	Q_OBJECT
public:

	/// Constructor for the element.  For the edgeList and emissionLineList, the edges and lines \em must be in order (ie: edges { K, L1, L2, L3, ... }, lines { Ka1, Ka2, ... }.
	explicit AMElement(QString name, QString symbol, QString atomicNumber, QStringList edgeList, QStringList emissionLineList, QObject *parent = 0);

	/// Returns the name of the element.
	QString name() const { return name_; }
	/// Returns the symbol of the element.
	QString symbol() const { return symbol_; }
	/// Returns the atomic number (as a QString) of the element.
	QString atomicNumber() const { return atomicNumber_; }
	/// Returns the entire list of edges.
	QList<QPair<QString, QString> > edges() const { return edges_; }
	/// Returns the entire list of emission lines.
	QList<QPair<QString, QString> > emissionLines() const { return emissionLines_; }

	// Helper functions that give access directly to a particular edge.
	/// Returns the K-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> KEdge() const { if (0 < edgeSize_) return edges_.at(0);  return qMakePair(QString(), QString()); }
	/// Returns the L1-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> L1Edge() const { if (1 < edgeSize_) return edges_.at(1);  return qMakePair(QString(), QString()); }
	/// Returns the L2-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> L2Edge() const { if (2 < edgeSize_) return edges_.at(2);  return qMakePair(QString(), QString()); }
	/// Returns the L3-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> L3Edge() const { if (3 < edgeSize_) return edges_.at(3);  return qMakePair(QString(), QString()); }
	/// Returns the M1-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> M1Edge() const { if (4 < edgeSize_) return edges_.at(4);  return qMakePair(QString(), QString()); }
	/// Returns the M2-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> M2Edge() const { if (5 < edgeSize_) return edges_.at(5);  return qMakePair(QString(), QString()); }
	/// Returns the M3-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> M3Edge() const { if (6 < edgeSize_) return edges_.at(6);  return qMakePair(QString(), QString()); }
	/// Returns the M4-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> M4Edge() const { if (7 < edgeSize_) return edges_.at(7);  return qMakePair(QString(), QString()); }
	/// Returns the M5-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> M5Edge() const { if (8 < edgeSize_) return edges_.at(8);  return qMakePair(QString(), QString()); }
	/// Returns the N1-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N1Edge() const { if (9 < edgeSize_) return edges_.at(9);  return qMakePair(QString(), QString()); }
	/// Returns the N2-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N2Edge() const { if (10 < edgeSize_) return edges_.at(10);  return qMakePair(QString(), QString()); }
	/// Returns the N3-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N3Edge() const { if (11 < edgeSize_) return edges_.at(11);  return qMakePair(QString(), QString()); }
	/// Returns the N4-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N4Edge() const { if (12 < edgeSize_) return edges_.at(12);  return qMakePair(QString(), QString()); }
	/// Returns the N5-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N5Edge() const { if (13 < edgeSize_) return edges_.at(13);  return qMakePair(QString(), QString()); }
	/// Returns the N6-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N6Edge() const { if (14 < edgeSize_) return edges_.at(14);  return qMakePair(QString(), QString()); }
	/// Returns the N7-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> N7Edge() const { if (15 < edgeSize_) return edges_.at(15);  return qMakePair(QString(), QString()); }
	/// Returns the O1-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> O1Edge() const { if (16 < edgeSize_) return edges_.at(16);  return qMakePair(QString(), QString()); }
	/// Returns the O2-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> O2Edge() const { if (17 < edgeSize_) return edges_.at(17);  return qMakePair(QString(), QString()); }
	/// Returns the O3-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> O3Edge() const { if (18 < edgeSize_) return edges_.at(18);  return qMakePair(QString(), QString()); }
	/// Returns the O4-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> O4Edge() const { if (19 < edgeSize_) return edges_.at(19);  return qMakePair(QString(), QString()); }
	/// Returns the O5-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> O5Edge() const { if (20 < edgeSize_) return edges_.at(20);  return qMakePair(QString(), QString()); }
	/// Returns the P1-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> P1Edge() const { if (21 < edgeSize_) return edges_.at(21);  return qMakePair(QString(), QString()); }
	/// Returns the P2-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> P2Edge() const { if (22 < edgeSize_) return edges_.at(22);  return qMakePair(QString(), QString()); }
	/// Returns the P3-edge.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> P3Edge() const { if (23 < edgeSize_) return edges_.at(23);  return qMakePair(QString(), QString()); }

	// Helper functions that give access directly to a particular edge.
	/// Returns the K-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Kalpha1() const { if (0 < emissionLineSize_) return emissionLines_.at(0);  return qMakePair(QString(), QString()); }
	/// Returns the K-alpha2 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Kalpha2() const { if (1 < emissionLineSize_) return emissionLines_.at(1);  return qMakePair(QString(), QString()); }
	/// Returns the K-beta1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Kbeta1() const { if (2 < emissionLineSize_) return emissionLines_.at(2);  return qMakePair(QString(), QString()); }
	/// Returns the L-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lalpha1() const { if (3 < emissionLineSize_) return emissionLines_.at(3);  return qMakePair(QString(), QString()); }
	/// Returns the L-alpha2 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lalpha2() const { if (4 < emissionLineSize_) return emissionLines_.at(4);  return qMakePair(QString(), QString()); }
	/// Returns the L-beta1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lbeta1() const { if (5 < emissionLineSize_) return emissionLines_.at(5);  return qMakePair(QString(), QString()); }
	/// Returns the L-beta2 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lbeta2() const { if (6 < emissionLineSize_) return emissionLines_.at(6);  return qMakePair(QString(), QString()); }
	/// Returns the L-gamma1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lgamma1() const { if (7 < emissionLineSize_) return emissionLines_.at(7);  return qMakePair(QString(), QString()); }
	/// Returns the M-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Malpha1() const { if (8 < emissionLineSize_) return emissionLines_.at(8);  return qMakePair(QString(), QString()); }
	/// Returns the K-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Kalpha() const { return Kalpha1(); }
	/// Returns the K-beta1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Kbeta() const { return Kbeta1(); }
	/// Returns the L-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lalpha() const { return Lalpha1(); }
	/// Returns the L-beta1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lbeta() const { return Lbeta1(); }
	/// Returns the L-gamma1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Lgamma() const { return Lgamma1(); }
	/// Returns the M-alpha1 emission line.  Returns a null pointer if the edge doesn't exist.
	QPair<QString, QString> Malpha() const { return Malpha1(); }

signals:

public slots:

private:
	// Member variables.
	// Name.
	QString name_;
	// Symbol.
	QString symbol_;
	// Atomic number saved as a string.
	QString atomicNumber_;
	// Name, Energy.
	QList<QPair<QString, QString> > edges_;
	// Name, Energy.
	QList<QPair<QString, QString> > emissionLines_;

	// Cached sizes of the lists for quick access.
	int edgeSize_;
	int emissionLineSize_;
};

#endif // ELEMENT_H
