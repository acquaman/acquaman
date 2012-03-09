#ifndef AM1DBASICDERIVATIVEABEDITOR_H
#define AM1DBASICDERIVATIVEABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AM1DDerivativeAB.h"

/*!
  This is a very basic view that contains a drop box that holds all of the available data sources and then sets the name for the data source to be analyzed.

  \todo Because of the dynamic nature of setting various data sources, the AMDataSource API will need to have a general capability for choosing data sources like this (or in some other way).  That way some of these views can be generalized rather than making a specific one for every analysis block.
  */
class AM1DBasicDerivativeABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AM1DDerivativeAB pointer.
	explicit AM1DBasicDerivativeABEditor(AM1DDerivativeAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles setting the name for the analysis block based on the new choice of input data source.
	void onNameChoiceChanged(int index);
	/// Helper slot.  Populates the names combo box.
	void populateComboBox();

protected:
	/// Pointer to the derivative analysis block.
	AM1DDerivativeAB *analysisBlock_;

	/// The combo box that contains all of the names.
	QComboBox *names_;
};

#endif // AM1DBASICDERIVATIVEABEDITOR_H
