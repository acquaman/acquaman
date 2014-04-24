#ifndef AMBEAMCONFIGURATION_H
#define AMBEAMCONFIGURATION_H

#include <QWidget>
#include <QVector3D>

class QLabel;
class QPushButton;
class AMBeamConfiguration;
class QVector3D;
class QCheckBox;
class QComboBox;
class QLineEdit;

#define AMBEAMCONFIGURATIONVIEW_CANNOT_ASSIGN_NULL_BEAM 594001
#define AMBEAMCONFIGURATIONVIEW_UPDATE_DATA_INVALID_FIRST_SHAPE 594002
#define AMBEAMCONFIGURATIONVIEW_UPDATE_DATA_INVALID_SECOND_SHAPE 594003

class AMBeamConfigurationView : public QWidget
{
    Q_OBJECT
public:
    AMBeamConfigurationView(AMBeamConfiguration *beam, QWidget *parent = 0);

public slots:
    void beamChanged(QObject*);
    void saveBeamConfiguration();
    void loadBeamConfiguration(int);

signals:
    void oneSelect();
    void twoSelect();
    void intersection();

protected:
    QCheckBox *divergentBox_;

    QLabel *oneOneX_;
    QLabel *oneOneY_;
    QLabel *oneOneZ_;

    QLabel *oneTwoX_;
    QLabel *oneTwoY_;
    QLabel *oneTwoZ_;

    QLabel *oneThreeX_;
    QLabel *oneThreeY_;
    QLabel *oneThreeZ_;

    QLabel *oneFourX_;
    QLabel *oneFourY_;
    QLabel *oneFourZ_;

    QLabel *twoOneX_;
    QLabel *twoOneY_;
    QLabel *twoOneZ_;

    QLabel *twoTwoX_;
    QLabel *twoTwoY_;
    QLabel *twoTwoZ_;

    QLabel *twoThreeX_;
    QLabel *twoThreeY_;
    QLabel *twoThreeZ_;

    QLabel *twoFourX_;
    QLabel *twoFourY_;
    QLabel *twoFourZ_;

    QPushButton *oneSelect_;
    QPushButton *twoSelect_;

    QPushButton *intersectionButton_;

    QComboBox *beamSelectionBox_;
    QLineEdit *beamConfigurationName_;
    QPushButton * beamSaveConfiguration_;

    AMBeamConfiguration* beamModel_;



    bool divergent_;

protected slots:
    void divergentChecked(int);
    void selectTwo();
    void updateData();
    void updateOneOne(QVector3D);
    void updateOneTwo(QVector3D);
    void updateOneThree(QVector3D);
    void updateOneFour(QVector3D);
    void updateTwoOne(QVector3D);
    void updateTwoTwo(QVector3D);
    void updateTwoThree(QVector3D);
    void updateTwoFour(QVector3D);
    void populateBeamSelectionBox();
};

#endif // AMBEAMCONFIGURATION_H
