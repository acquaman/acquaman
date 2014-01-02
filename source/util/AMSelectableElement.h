#ifndef AMSELECTABLEELEMENT_H
#define AMSELECTABLEELEMENT_H

#include "util/AMElement.h"

/// This class extends the basic AMElement class to provide a means of "choosing" or "selecting" emission lines and absorption edges and keeping a memory of them.
class AMSelectableElement : public AMElement
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid element name (eg: Iron).  Will become a null object if the name is invalid.
 	virtual ~AMSelectableElement();
	AMSelectableElement(const QString &elementName, QObject *parent = 0);
	/// Constructor.  Takes an existing AMElement and builds an AMSelectableElement out of it.
	AMSelectableElement(const AMElement &element, QObject *parent);
	/// Copy constructor.
	AMSelectableElement(const AMSelectableElement &original);
	/// Null constructor.
	AMSelectableElement();

	/// Returns whether there are any selected absorption edges or any selected emission lines.
	bool hasSelectedItems() const { return !(selectedAbsorptionEdges_.isEmpty() && selectedEmissionLines_.isEmpty()); }
	/// Returns whether there are any selected absorption edges.
	bool hasSelectedAbsorptionEdges() const { return !selectedAbsorptionEdges_.isEmpty(); }
	/// Returns whether there are any selected emission lines.
	bool hasSelectedEmissionLines() const { return !selectedEmissionLines_.isEmpty(); }

	/// Returns the list of selected absorption edges.
	QList<AMAbsorptionEdge> selectedAbsorptionEdges() const { return selectedAbsorptionEdges_; }
	/// Returns the list of selected emission lines.
	QList<AMEmissionLine> selectedEmissionLines() const { return selectedEmissionLines_; }

	/// Returns the index of the given absorption edge, if it has been selected.
	int indexOfSelectedAbsorptionEdge(const AMAbsorptionEdge &edge) const { return selectedAbsorptionEdges_.indexOf(edge); }
	/// Returns the index of the given emission line, if it has been selected.
	int indexOfSelectedEmissionLine(const AMEmissionLine &line) const { return selectedEmissionLines_.indexOf(line); }
	/// Returns whether the given absorption edge has been selected.
	bool isSelected(const AMAbsorptionEdge &edge) const { return selectedAbsorptionEdges_.contains(edge); }
	/// Returns whether the given emission line has been selected.
	bool isSelected(const AMEmissionLine &line) const { return selectedEmissionLines_.contains(line); }

signals:
	/// Notifies that an edge has been selected.
	void absorptionEdgeSelected(const AMAbsorptionEdge &);
	/// Notifies that an edge has been deselected.
	void absorptionEdgeDeselected(const AMAbsorptionEdge &);
	/// Notifies that an emission line has been selected.
	void emissionLineSelected(const AMEmissionLine &);
	/// Notifies that an emission line has been deselected.
	void emissionLineDeselected(const AMEmissionLine &);
	/// Notifies that all edges were deselected.
	void allAbsorptionEdgesDeselected();
	/// Notifies that all emission lines were deselected.
	void allEmissionLinesDeselected();

public slots:
	/// Adds a selected absorption edge.
	bool selectAbsorptionEdge(const AMAbsorptionEdge &edge);
	/// Removes a selected absorption edge.
	bool deselectAbsorptionEdge(const AMAbsorptionEdge &edge);
	/// Adds a selected emission line.
	bool selectEmissionLine(const AMEmissionLine &line);
	/// Removes a selected emission line.
	bool deselectEmissionLine(const AMEmissionLine &line);
	/// Remove all absorption edges from the selected absorption edge list.
	void removeAllAbsorptionEdges();
	/// Remove all emission lines from the selected emission line list.
	void removeAllEmissionLines();
	/// Remove all selected items for the element.
	void removeAllSelectedItems();

protected:
	/// List of selected absorption edges.
	QList<AMAbsorptionEdge> selectedAbsorptionEdges_;
	/// List of selected emission lines.
	QList<AMEmissionLine> selectedEmissionLines_;
};

#endif // AMSELECTABLEELEMENT_H
