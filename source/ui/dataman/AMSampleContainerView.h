#ifndef AMSAMPLECONTAINERVIEW_H
#define AMSAMPLECONTAINERVIEW_H

/*
#include <QListView>

class AMSampleContainer;
class AMSampleView;

/// This class is a view for a list of AMSamples
/// uses index changed and setCurrentSelection to coordinate
/// selected sample with AMSampleCamera
class AMSampleContainerView : public QListView
{
    Q_OBJECT
public:
    /// Constructors
    AMSampleContainerView(QWidget* parent = 0);
    AMSampleContainerView(AMSampleContainer* sampleContainer, QWidget *parent = 0);

    /// set the sample container to visualize
    void setSampleContainer(AMSampleContainer* sampleContainer);

    /// called when the current index is changed
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public slots:
    /// sets the current selection
    void setCurrentSelection(int);

//    void updateSampleName(int, QString);

signals:
    /// signal emitted when the current index has been changed
    void indexChanged(int);

protected:
    /// initialization
    void init();
protected:
    /// the sample container
    AMSampleContainer* sampleContainer_;
    /// the view for individual samples
    AMSampleView* sampleView_;
};
*/

#endif // AMSAMPLECONTAINERVIEW_H
