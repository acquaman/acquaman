#ifndef AMGRAPHICSVIEWWIZARD_H
#define AMGRAPHICSVIEWWIZARD_H

#include <QWizard>

class AMShapeDataSetGraphicsView;
class QPointF;
class QTimer;
class QFrame;
class QLabel;
class QVector3D;

class AMGraphicsViewWizard : public QWizard
{
    Q_OBJECT
public:
    enum {Wizard_Title, Help_Title, Title, Text, Help, Other, Default};
    AMGraphicsViewWizard(QWidget* parent = 0);

    AMShapeDataSetGraphicsView* view();

    QPointF scale();

    void setScale(QPointF scale);
    void setScale(double scaleFactor);



    virtual QList<QVector3D*>* coordinateList() = 0;

    virtual QString message(int type);

    QPointF mapPointToVideo(QPointF);


public slots:
    void setView(AMShapeDataSetGraphicsView* view);

    virtual void showHelp();

    virtual void addPoint(QPointF position) = 0;

    void updateScene(AMShapeDataSetGraphicsView* view);
    void outputItems();
    void fixText();

protected:




protected:
    AMShapeDataSetGraphicsView* view_;
    QPointF* scale_;
    int numberOfPoints_;

};


class AMWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    enum {Title, Text, Help, Other, Default};
    AMWizardPage(QWidget* parent = 0);
    AMGraphicsViewWizard* viewWizard() const;
    virtual QString message(int type);
public slots:
    void setLabelText(QString text);
protected:
    QLabel* topLabel_;

};


class AMWaitPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMWaitPage(QWidget* parent = 0);
    bool isComplete() const;
    void stopTimer();
    void startTimer(int msec);
protected slots:
    void nextPage();


private:
    QTimer* waitTimer_;


};


class AMViewPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMViewPage(QWidget* parent = 0);

    AMShapeDataSetGraphicsView* view();

public slots:
    void setView(AMShapeDataSetGraphicsView* view);

    void initializePage();
    void cleanupPage();


protected slots:
    void addView();

private:
    AMShapeDataSetGraphicsView* view_;

    QLayout* layout_;
    QFrame* viewFrame_;

};


#endif // AMGRAPHICSVIEWWIZARD_H
