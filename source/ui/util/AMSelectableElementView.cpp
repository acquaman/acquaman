#include "AMSelectableElementView.h"

// AMSelectableAbsorptionEdgeView
////////////////////////////////////////////////

AMSelectableAbsorptionEdgeView::AMSelectableAbsorptionEdgeView(const AMAbsorptionEdge &edge, QWidget *parent)
	: AMSelectableItemView(parent)
{

}

// AMSelectableEmissionLineView
////////////////////////////////////////////////

AMSelectableEmissionLineView::AMSelectableEmissionLineView(const AMEmissionLine &line, QWidget *parent)
	: AMSelectableItemView(parent)
{

}

// AMSelectableElementView
////////////////////////////////////////////////

AMSelectableElementView::AMSelectableElementView(QWidget *parent)
	: QWidget(parent)
{
}
