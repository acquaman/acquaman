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
	/// Sets the view with a new line.
	void setLine(QPair<QString, QString> line)
	{
		if (line.first.isEmpty() || line.second.compare("-") == 0)
			hide();
		else
			show();

		line_ = line;
		checkBox_->setChecked(false);
		lineLabel_->setText(line.first + ": " + line.second + " eV");
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
	explicit VESPERSXRFElementView(QWidget *parent = 0);
	explicit VESPERSXRFElementView(AMElement *el, QWidget *parent = 0);

signals:
	/// Signal emitted when a custom ROI should be created.
	void addCustomROI();
	/// Signal emitted with current element and the selected emission line to be added.
	void addROI(AMElement *, QPair<QString, QString>);
	/// Signal emitted with the current element and selected emission line to be removed.
	void removeROI(AMElement *, QPair<QString, QString>);

public slots:
	/// Sets the element to view.  Handles all the layout properties of the dialog.
	void setElement(AMElement *el);

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
};

#endif // VESPERSXRFELEMENTVIEW_H
