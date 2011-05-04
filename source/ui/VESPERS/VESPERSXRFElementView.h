#ifndef VESPERSXRFELEMENTVIEW_H
#define VESPERSXRFELEMENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QCheckBox>
#include <QList>

#include "util/AMElement.h"

class LineView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a Line in the form of a QPair<QString, QString> and creates a custom line widget.
	LineView(QPair<QString, QString> line, QWidget *parent = 0)
		: QWidget(parent)
	{
		line_ = line;
		lineLabel_ = new QLabel(line.first + " " + line.second + " eV");
		checkBox_ = new QCheckBox();

		connect(checkBox_, SIGNAL(stateChanged(int)), this, SLOT(onChecked()));

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
	QPair<QString, QString> line() const { return line_; }
	/// Returns the line in the form of a QLabel.
	QLabel *lineLabel() const { return lineLabel_; }
	/// Sets the view with a new line.  Does not emit any signals while setting the all the values.
	void setLine(QPair<QString, QString> line, bool alreadyChecked = false)
	{
		blockSignals(true);
		if (line.first.isEmpty() || line.second.compare("-") == 0)
			hide();
		else
			show();

		line_ = line;
		checkBox_->setChecked(alreadyChecked);
		lineLabel_->setText(line.first + ": " + line.second + " eV");
		blockSignals(false);
	}

signals:
	/// This signal is emitted when the check box is checked/unchecked.  It contains the line for reference purposes.
	void lineChecked(LineView *);

private slots:
	/// Helper slot used to emit the lineChecked signal.
	void onChecked() { emit lineChecked(this); }

private:
	// Member variables.
	QCheckBox *checkBox_;
	QLabel *lineLabel_;
	QPair<QString, QString> line_;
};

class VESPERSXRFElementView : public QWidget
{
	Q_OBJECT
public:
	/// This constructor builds an element view for the fluorescence detector.  Only the emission lines are displayed.
	explicit VESPERSXRFElementView(double minEnergy = 0, double maxEnergy = 1e6, QWidget *parent = 0);
	explicit VESPERSXRFElementView(AMElement *el, double minEnergy = 0, double maxEnergy = 1e6, QWidget *parent = 0);

	/// Returns the minimum energy.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy.
	double maximumEnergy() const { return maximumEnergy_; }
	/// Returns the element.
	AMElement *element() const { return element_; }

signals:
	/// Signal emitted when a custom ROI should be created.
	void addCustomROI();
	/// Signal emitted with current element and the selected emission line to be added.
	void addROI(AMElement *, QPair<QString, QString>);
	/// Signal emitted with the current element and selected emission line to be removed.
	void removeROI(AMElement *, QPair<QString, QString>);

public slots:
	/// Sets the element to view.  Handles all the layout properties of the dialog.
	void setElement(AMElement *el, QList<QPair<int, QString> > checked);

	/// Sets the minimum energy.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; fillEmissionLines(element_); }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; fillEmissionLines(element_); }

private slots:
	/// Convenience slot that emits the addROI signal when a line is checked or removeROI if the line is unchecked.
	void emitLines(LineView *line);

private:
	/// Fills the emission lines group box with the emission lines of the given Element.
	void fillEmissionLines(AMElement *el);

	// Member variables.
	QLabel *name_;
	QLabel *number_;
	QLabel *symbol_;
	AMElement *element_;

	// Lines.
	QList<LineView *> lines_;
	// Checked lines.
	QList<QPair<int, QString> > checked_;

	// Holds the minimum energy.  This is the lower limit and elements that don't have emission lines with energies higher then this are disabled.
	double minimumEnergy_;
	// Holds the maximum energy.  This is the upper limit and elements that don't have emission lines with energies lower then this are disabled.
	double maximumEnergy_;
};

#endif // VESPERSXRFELEMENTVIEW_H
