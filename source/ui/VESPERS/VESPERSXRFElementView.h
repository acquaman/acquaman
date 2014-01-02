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


#ifndef VESPERSXRFELEMENTVIEW_H
#define VESPERSXRFELEMENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QCheckBox>
#include <QList>

#include "util/VESPERS/XRFElement.h"

class LineView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a Line in the form of a QPair<QString, QString> and creates a custom line widget.
 	virtual ~LineView();
	LineView(AMEmissionLine line = AMEmissionLine(), QWidget *parent = 0)
		: QWidget(parent)
	{
		lineLabel_ = new QLabel;
		checkBox_ = new QCheckBox;

		connect(checkBox_, SIGNAL(stateChanged(int)), this, SLOT(onChecked()));

		setLine(line);

		QHBoxLayout *layout = new QHBoxLayout;
		layout->addWidget(checkBox_);
		layout->addWidget(lineLabel_);
		layout->addStretch();
		setLayout(layout);
		setMaximumHeight(35);
	}

	/// Returns whether the box is checked or not.
	bool checked() const { return checkBox_->isChecked(); }

	/// Returns the line as a QPair<QString, QString>
	AMEmissionLine line() const { return line_; }
	/// Returns the line in the form of a QLabel.
	QLabel *lineLabel() const { return lineLabel_; }
	/// Sets the view with a new line.  Does not emit any signals while setting the all the values.
	void setLine(AMEmissionLine line, bool alreadyChecked = false)
	{
		blockSignals(true);
		setVisible(!line.isNull());
		line_ = line;
		checkBox_->setChecked(alreadyChecked);
		lineLabel_->setText(line.greekLineName() + ": " + line.energyString() + " eV");
		blockSignals(false);
	}

signals:
	/// This signal is emitted when the check box is checked/unchecked.  It contains the state of the check box and the name of the line for reference purposes.
	void lineCheckedChanged(bool, QString);

private slots:
	/// Helper slot used to emit the lineChecked signal.
	void onChecked() { emit lineCheckedChanged(checkBox_->isChecked(), line_.greekLineName()); }

private:
	// Member variables.
	QCheckBox *checkBox_;
	QLabel *lineLabel_;
	AMEmissionLine line_;
};

/*! Builds a view for an XRFElement.
  */
class VESPERSXRFElementView : public QWidget
{
	Q_OBJECT
public:
	/// This constructor builds an element view for the fluorescence detector.  Only the emission lines are displayed.
 	virtual ~VESPERSXRFElementView();
	explicit VESPERSXRFElementView(XRFElement *el = 0, QWidget *parent = 0);

	/// Returns the element.
	XRFElement *element() const { return element_; }
	/// Returns the minimum energy used by this view for display logic.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy used by this view for display logic.
	double maximumEnergy() const { return maximumEnergy_; }

public slots:
	/// Sets the element to view.  Handles all the layout properties of the dialog.
	void setElement(XRFElement *el);
	/// Sets the minimum energy.  Determines what should be shown based on the new value.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; if (element_) fillEmissionLines(); }
	/// Sets the maximum energy.  Determines what should be shown based on the new value.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; if (element_) fillEmissionLines(); }

signals:
	/// Notifies that a line has been chosen to be added.
	void addLine(QString);
	/// Notifies that a line has been chosen to be removed.
	void removeLine(QString);

protected slots:
	/// Handles when checked state changes from LineView.  Takes in whether the line view was checked or unchecked and passes on the name of the line.
	void onLineCheckedChanged(bool checked, QString lineName);
	/// Fills the emission lines group box with the emission lines based on the current element_.  If the energy of the line is not within the minimum and maximum energy of the element then it does not show that line.
	void fillEmissionLines();

protected:
	/// Function that returns true if a given energy is acceptable to be added.
	bool energyWithinTolerance(double energy) { return (energy >= minimumEnergy_ && energy <= maximumEnergy_) ? true : false; }

	// Member variables.

	/// Labels.
	QLabel *name_;
	QLabel *number_;
	QLabel *symbol_;

	/// Pointer to the current element being viewed.
	XRFElement *element_;

	/// Line views.
	QList<LineView *> lines_;

	/// Holds the minimum energy used for displaying emission lines.
	double minimumEnergy_;
	/// Holds the maximum energy used for displaying emission lines.
	double maximumEnergy_;

};

#endif // VESPERSXRFELEMENTVIEW_H
