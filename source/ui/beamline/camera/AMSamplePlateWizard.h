#ifndef AMSAMPLEPLATEWIZARD_H
#define AMSAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class QSlider;
class QGraphicsRectItem;



// changing it to determine the exact sample plate position

class AMSamplePlateWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
	enum
	{
                  Page_Intro = 0, // description -> should skip on start
                  Page_Check = 1,  // check to see if the plate is where it should be
                  Page_Final = 2,
                  Page_Option = 3,
                  Page_Free = 4
	 };
    AMSamplePlateWizard(QWidget* parent = 0);
    virtual ~AMSamplePlateWizard();

	virtual int nextId() const;

	virtual void waitPage();

    virtual QString message(int type);

    double currentRotation();

protected:

    double requestMotorRotation();



public slots:
    void back();
	virtual void addPoint(QPointF position);

	/// Will add duplicate points unless this is called when leaving the page
	void removePoint(QPointF* point);

	void setCurrentRotation(double rotation);



signals:
    void movePlate(int);
    void requestRotation();

protected:
	double coordinateX(int id);
	double coordinateY(int id);
	double coordinateZ(int id);
	void addResetPointsButton(int id);
	QPen getDefaultPen();

protected slots:
	void triggerReset(int id);
	void addResetPage(int id);
	void showResetButton(int id);

private:
	QVector<int> resetPages_;
	QVector<QGraphicsRectItem*> samplePointShapes_;
	double currentRotation_;


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
