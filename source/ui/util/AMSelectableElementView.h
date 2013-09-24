#ifndef AMSELECTABLEELEMENTVIEW_H
#define AMSELECTABLEELEMENTVIEW_H

#include <QWidget>

#include "ui/util/AMSelectableItemView.h"
#include "util/AMSelectableElement.h"

/// Sbuclass of the AMSelectableItemView for some specific AMAbsorptionEdge additions.
class AMSelectableAbsorptionEdgeView : public AMSelectableItemView
{
	Q_OBJECT

public:
	explicit AMSelectableAbsorptionEdgeView(const AMAbsorptionEdge &edge, QWidget *parent = 0);
};

/// Subclass of the AMSelectableItemView for some specific AMEmissionLine additions.
class AMSelectableEmissionLineView : public AMSelectableItemView
{
	Q_OBJECT

public:
	explicit AMSelectableEmissionLineView(const AMEmissionLine &line, QWidget *parent = 0);
};

/// The selectable element view where you can select emission lines and absorption edges.
class AMSelectableElementView : public QWidget
{
	Q_OBJECT
public:
	explicit AMSelectableElementView(QWidget *parent = 0);

signals:

public slots:

};

#endif // AMSELECTABLEELEMENTVIEW_H
