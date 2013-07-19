#ifndef AMGRAPHICSVIEWWIZARD_H
#define AMGRAPHICSVIEWWIZARD_H

#include <QWizard>

class AMShapeDataSetGraphicsView;
class QPointF;
class QTimer;

class AMGraphicsViewWizard : public QWizard
{
    Q_OBJECT
public:
    AMGraphicsViewWizard(QWidget* parent = 0);

    AMShapeDataSetGraphicsView* view();

    QPointF scale();

    void setScale(QPointF scale);
    void setScale(double scaleFactor);

public slots:
    void setView(AMShapeDataSetGraphicsView* view);

private:
    AMShapeDataSetGraphicsView* view_;
    QPointF* scale_;

};

class AMWaitPage : public QWizardPage
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


#endif // AMGRAPHICSVIEWWIZARD_H
