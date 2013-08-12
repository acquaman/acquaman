#ifndef AMSAMPLECONTAINERVIEW_H
#define AMSAMPLECONTAINERVIEW_H

#include <QListView>

class AMSampleContainer;
class AMSampleEthanView;

class AMSampleContainerView : public QListView
{
public:
    AMSampleContainerView(QWidget* parent = 0);
    AMSampleContainerView(AMSampleContainer* sampleContainer, QWidget *parent = 0);

    void setSampleContainer(AMSampleContainer* sampleContainer);

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    void init();
protected:
    AMSampleContainer* sampleContainer_;
    AMSampleEthanView* sampleView_;
};

#endif // AMSAMPLECONTAINERVIEW_H
