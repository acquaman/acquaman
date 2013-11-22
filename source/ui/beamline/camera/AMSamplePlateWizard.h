#ifndef AMSAMPLEPLATEWIZARD_H
#define AMSAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class QSlider;
class QGraphicsRectItem;


/// Wizard for repositioning the Sample plate.
/// used to adjust it based on its exact position.
// changing it to use to determine the exact sample plate position
/// \todo everything
class AMSamplePlateWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
	enum
	{
		  Page_Intro, // description -> should skip on start
		  Page_Check,  // check to see if the plate is where it should be
		  Page_Wait_One,
		  Page_Set_One,
		  Page_Wait_Two,
		  Page_Set_Two,
		  Page_Final,
		  Page_Option
	 };
    AMSamplePlateWizard(QWidget* parent = 0);
    ~AMSamplePlateWizard();

    int nextId() const;

    void waitPage();

	int relativeId();/// \todo need this to differentiate the wait and set pages

	virtual QString message(int type);

public slots:
    void back();
	/// \todo get rid of this
    void sliderChanged();
	/// \todo implement addPoint
	virtual void addPoint(QPointF position);
	void removePoint(QPointF* point);
	/// Will add duplicate points unless this is called when leaving the page




signals:
    void movePlate(int);

protected:
	double coordinateX(int id);
	double coordinateY(int id);
	double coordinateZ(int id);
	void addResetPointsButton(int id);

protected slots:
	void triggerReset(int id);
	void addResetPage(int id);
	void showResetButton(int id);

private:
	QVector<int> resetPages_;
	QVector<QGraphicsRectItem*> samplePointShapes_;


};

class AMSampleIntroPage : public AMWizardPage
{
	Q_OBJECT
public:
	void initializePage();

	void timerEvent(QTimerEvent *event);

};

class AMSampleWaitPage : public AMWaitPage
{
    Q_OBJECT
public:
    void initializePage();
};

class AMSampleSetPage : public AMViewPage
{
    Q_OBJECT
public:
//    AMSampleSetPage(QWidget* parent = 0);
	void initializePage();

public slots:

	void addPoint(QPointF position);
	/// add a point to the page's list of points
	void insertPoint(QPointF *position);
	void disconnectMouseSignal();
	/// clears all the points currently selected by the page.
	void resetPoints();

private:
	/// these are the points associated with a particular page.
	QVector<QPointF*> localPoints_;
};

class AMSampleCheckPage : public AMCheckPage
{
    Q_OBJECT
protected slots:
    virtual void checkBoxChanged(bool state);
};

#endif // AMSAMPLEPLATEWIZARD_H
