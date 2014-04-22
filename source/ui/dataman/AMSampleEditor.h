#ifndef AMSAMPLEEDITOR_H
#define AMSAMPLEEDITOR_H

#include <QWidget>

class QLabel;
class QPushButton;

class AMSample;
class AMSamplePre2013;
class AMDatabase;

class AMSampleEditor : public QWidget
{
Q_OBJECT
public:
	AMSampleEditor(AMDatabase *database, QWidget *parent = 0);
	virtual ~AMSampleEditor();

	void setSample(const AMSample *sample);
	void setSamplePre2013(const AMSamplePre2013 *samplePre2013);

protected slots:
	void onChangeSampleButtonClicked();

protected:
	AMDatabase *database_;
	const AMSample *currentSample_;
	const AMSamplePre2013 *currentSamplePre2013_;

	QLabel *nameLabel_;
	QLabel *createdLabel_;
	QPushButton *changeSampleButton_;
};

#endif // AMSAMPLEEDITOR_H
