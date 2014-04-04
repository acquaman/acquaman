#ifndef AMSAMPLEBRIEFVIEW_H
#define AMSAMPLEBRIEFVIEW_H

#include <QWidget>

class QLabel;
class QPushButton;

class AMSample;

class AMSampleBriefView : public QWidget
{
Q_OBJECT
public:
	/// Constructor takes a sample to view (or a null pointer)
	AMSampleBriefView(AMSample *sample, QWidget *parent = 0);

protected:
	/// The sample we intend to view
	AMSample *sample_;

	/// Label that holds the name of the sample
	QLabel *nameLabel_;
	/// Push button for launching the expanded sample view
	QPushButton *showSampleInformationButton_;
};

#endif // AMSAMPLEBRIEFVIEW_H
