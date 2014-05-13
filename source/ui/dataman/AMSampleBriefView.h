#ifndef AMSAMPLEBRIEFVIEW_H
#define AMSAMPLEBRIEFVIEW_H

#include <QWidget>

class QLabel;
class QPushButton;

class AMSample;
class AMSampleReadOnlyView;

class AMSampleBriefView : public QWidget
{
Q_OBJECT
public:
	/// Constructor takes a sample to view (or a null pointer)
	AMSampleBriefView(const AMSample *sample = 0, QWidget *parent = 0);

public slots:
	/// Sets the sample
	void setSample(const AMSample *sample);

protected slots:
	/// Handles clicks from the button and launches a new AMSampleView
	void onShowSampleInformationButtonClicked();

protected:
	/// The sample we intend to view
	const AMSample *sample_;

	/// Label that holds the name of the sample
	QLabel *nameLabel_;
	/// Push button for launching the expanded sample view
	QPushButton *showSampleInformationButton_;

	/// A full AMSampleView that is launched (or shown) when the show information button is clicked
	AMSampleReadOnlyView *sampleReadOnlyView_;
};

#endif // AMSAMPLEBRIEFVIEW_H
