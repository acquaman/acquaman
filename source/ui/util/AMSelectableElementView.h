#ifndef AMSELECTABLEELEMENTVIEW_H
#define AMSELECTABLEELEMENTVIEW_H

#include <QWidget>

#include "ui/util/AMSelectableItemView.h"
#include "util/AMSelectableElement.h"
#include "util/AMNameAndRangeValidator.h"

#include <QGroupBox>
#include <QScrollArea>

/// Sbuclass of the AMSelectableItemView for some specific AMAbsorptionEdge additions.
class AMSelectableAbsorptionEdgeView : public AMSelectableItemView
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMAbsorptionEdge as an argument and builds the view.  If the edge is null, it auto-hides itself.
 	virtual ~AMSelectableAbsorptionEdgeView();
	explicit AMSelectableAbsorptionEdgeView(const AMAbsorptionEdge &edge = AMAbsorptionEdge(), QWidget *parent = 0);

	/// Returns the absorption edge that this view is representing.
	const AMAbsorptionEdge &absorptionEdge() const { return edge_; }

signals:
	/// More verbose signal that provides the AbsorptionEdge as well as the selected state.
	void absorptionEdgeSelected(bool, const AMAbsorptionEdge &);

public slots:
	/// Sets the view with a new absorption edge and what the default checked state should be.  If the edge is null, then the view will auto-hide itself.
	void setAbsorptionEdge(const AMAbsorptionEdge &edge, bool alreadySelected = false);

protected slots:
	/// Handles emitting the signal when the item selected signal is emitted.
	void onItemSelectedChanged(bool isSelected);

protected:
	/// The current absorption edge this view is representing.
	AMAbsorptionEdge edge_;
};

/// Subclass of the AMSelectableItemView for some specific AMEmissionLine additions.  If the emission line is null, it auto-hides itself.
class AMSelectableEmissionLineView : public AMSelectableItemView
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMEmissionLine as an argument and builds the view.  If the emission line is null, it auto-hides itself.
 	virtual ~AMSelectableEmissionLineView();
	explicit AMSelectableEmissionLineView(const AMEmissionLine &line = AMEmissionLine(), QWidget *parent = 0);

	/// Returns the emission line that this view is representing.
	const AMEmissionLine &emissionLine() const { return emissionLine_; }

signals:
	/// More verbose signal that provides the AMEmissionLine as well as the selected state.
	void emissionLineSelected(bool, const AMEmissionLine &);

public slots:
	/// Sets the view with a new emission line and what the default checked state should be.  IF the emission line is null, then the view will auto-hide itself.
	void setEmissionLine(const AMEmissionLine &line, bool alreadySelected = false);

protected slots:
	/// Handles emitting the signal when the item selected signal is emitted.
	void onItemSelectedChanged(bool isSelected);

protected:
	/// The current emission line this view is representing.
	AMEmissionLine emissionLine_;
};

/// The selectable element view where you can select emission lines and absorption edges.
/*!
	This class attempts to provide a general view for looking at an element.  You can customize whether to look
	at absorption edges or emission lines or both.  You can set an energy range
	to reduce the number of edges and lines that are displayed, as well as providing
	some name filters to further reduce the number of edges and lines displayed.
	Of course, there is still a way to look at all the lines and edges is that is
	the desired functionality.
  */
class AMSelectableElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds an element view around the element provided.  If the element pointer is zero then it will build the view, but not initialize any values.
 	virtual ~AMSelectableElementView();
	explicit AMSelectableElementView(AMSelectableElement *element, QWidget *parent = 0);

	/// Returns the current element that this view is representing.
	AMSelectableElement *element() const { return element_; }
	/// Returns the energy range used for filtering.  If no range has been provided the range is null.
	const AMRange &energyRange() const { return absorptionEdgeValidator_->range(); }
	/// Returns the minimum energy in the energy range filter.
	double minimumEnergy() const { return absorptionEdgeValidator_->minimum(); }
	/// Returns the maximum energy in the energy range filter.
	double maximumEnergy() const { return absorptionEdgeValidator_->maximum(); }

	/// Changes the visibility of the emission line view.
	void setEmissionLineVisibility(bool visible);
	/// Changes the visibility of the absorption edge view.
	void setAbsorptionEdgeVisibility(bool visible);

	/// Returns the list of absorption edge name filters that have been provided to the element view.
	QList<QRegExp> absorptionEdgeNameFilters() const { return absorptionEdgeValidator_->nameFilters(); }
	/// Returns the absorption edge name filter at a given index.
	const QRegExp &absorptionEdgeNameFilterAt(int index) const { return absorptionEdgeValidator_->nameFilterAt(index); }
	/// Removes the absorption edge name filter at the given index.  Returns whether the removal was successful.
	bool removeAbsorptionEdgeNameFilter(int index);
	/// Removes the absorption edge name filter with the given name filter.  Returns whether the removal was successful.
	bool removeAbsorptionEdgeNameFilter(const QRegExp &filter);
	/// Adds a new absorption edge name filter to the list of name filters.
	void addAbsorptionEdgeNameFilter(const QRegExp &newNameFilter);

	/// Returns the list of emission line name filters that have provided to the element view.
	QList<QRegExp> emissionLineNameFilters() const { return emissionLineValidator_->nameFilters(); }
	/// Returns the emission line name filter at a given index.
	const QRegExp &emissionLineNameFilterAt(int index) const { return emissionLineValidator_->nameFilterAt(index); }
	/// Removes the emission line name filter at the given index.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(int index);
	/// Removes the emission line name filter with the given name fitler.  Returns whether the removal was successful.
	bool removeEmissionLineNameFilter(const QRegExp &filter);
	/// Adds a new emission line name filter to the list of name filters.
	void addEmissionLineNameFilter(const QRegExp &newNameFilter);

signals:

public slots:
	/// Sets a new AMSelectableElement to the view.  If a null pointer is passed, then the view will simply be uninitialized.
	void setElement(AMSelectableElement *element);
	/// Sets the energy range filter for this view.
	void setEnergyRange(const AMRange &newRange);
	/// Sets the energy range filter for this view.
	void setEnergyRange(double minimum, double maximum);
	/// Sets a new minimum value for the energy range.
	void setMinimumEnergy(double newMinimum);
	/// Sets a new maximum value for the energy range.
	void setMaximumEnergy(double newMaximum);

	/// Updates the list of absorption line views based on the current filters.
	void updateAbsorptionEdgeViewList();
	/// Updates the list of emission line views based on the current filters.
	void updateEmissionLineViewList();

protected slots:
	/// Handles when the check state from an AMSelectableAbsorptionEdgeView changes.
	void onAbsorptionEdgeSelected(bool isSelected, const AMAbsorptionEdge &edge);
	/// Handles when the check state from an AMSelectableEmissionLineView changes.
	void onEmissionLineSelected(bool isSelected, const AMEmissionLine &line);

protected:
	/// The label for the name of the element.
	QLabel *elementName_;
	/// The label for the symbol of the element.
	QLabel *elementSymbol_;
	/// The list of absorption edge views.
	QList<AMSelectableAbsorptionEdgeView *> absorptionEdgeViews_;
	/// The list of the emission line views.
	QList<AMSelectableEmissionLineView *> emissionLineViews_;
	/// The emission line group box.
	QGroupBox *emissionLineGroupBox_;
	/// The emission line scroll area.
	QScrollArea *emissionLineScrollArea_;
	/// The absorption edge group box.
	QGroupBox *absorptionEdgeGroupBox_;
	/// The absorption edge scroll area.
	QScrollArea *absorptionEdgeScrollArea_;

	/// The pointer to the element we are representing.
	AMSelectableElement *element_;
	/// The absorption edge validator.
	AMNameAndRangeValidator *absorptionEdgeValidator_;
	/// The emission line validator.
	AMNameAndRangeValidator *emissionLineValidator_;

	/// The energy range filter.
	AMRange energyRange_;
	/// The list of name filters used for knowing what absorption edges to show.
	QStringList absorptionEdgeNameFilters_;
	/// The list of name filters used for knowing what emission lines to show.
	QStringList emissionLineNameFilters_;
};

#endif // AMSELECTABLEELEMENTVIEW_H
