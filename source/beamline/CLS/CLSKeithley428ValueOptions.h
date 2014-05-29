//#ifndef CLSKEITHLEY428VALUEOPTIONS_H
//#define CLSKEITHLEY428VALUEOPTIONS_H

//#include <QObject>
//#include <QVector>
//#include <QStringList>
//#include <QDebug>

//#include "math.h"

//class CLSKeithley428ValueOptions : public QObject
//{
//    Q_OBJECT

//public:
//    /// Constructor.
//    explicit CLSKeithley428ValueOptions(QObject *parent = 0);
//    /// Destructor.
//    ~CLSKeithley428ValueOptions();

//    /// Returns the vector array storing the possible gain values.
//    QList<double> gainValues() const;
//    /// Returns a QStringList of the gainValues suitable for a view to display.
//    QStringList gainList() const;
//    /// Returns the vector array storing the possible gain units.
//    QStringList gainUnits() const;

//    /// Returns the vector array storing the possible sensitivity values.
//    QList<double> sensitivityValues() const;
//    /// Returns a QStringList of the sensitivity values suitable for a view to display.
//    QStringList sensitivityList() const;
//    /// Returns the vector array storing the possible sensitivity units.
//    QStringList sensitivityUnits() const;

//    /// Returns the gain value at the given index.
//    double gainValueAt(int index);
//    /// Returns the gain units at the given index.
//    QString gainUnitsAt(int index) const;

//    /// Returns the sensitivity value at the given index.
//    double sensitivityValueAt(int index);
//    /// Returns the sensitivity unit at the given index.
//    QString sensitivityUnitsAt(int index) const;

//    /// Returns the maximum gain value.
//    double gainValueMax();
//    /// Returns the maximum sensitivity value.
//    double gainValueMin();

//    /// Returns the minimum gain value.
//    double sensitivityValueMax();
//    /// Returns the minimum sensitivity value.
//    double sensitivityValueMin();

//    /// Returns the index of the maximum gain value.
//    int gainValueIndexMax();
//    /// Returns the index of the minimum gain value.
//    int gainValueIndexMin();

//    /// Returns true if the given index is within the acceptable range of gain values.
//    bool gainValueIndexOkay(int index);
//    /// Returns true if the given index is within the acceptable range of sensitivity values.
//    bool sensitivityValueIndexOkay(int index);

//    /// Returns true if the given index corresponds to a gain unit.
//    bool gainUnitIndexOkay(int index) const;
//    /// Returns true if the given index corresponds to a sensitivity unit.
//    bool sensitivityUnitIndexOkay(int index) const;

//    /// Adds the given value to the list of gains, and its sensitivity counterpart to the list of sensitivities.
//    void addGainValue(double voltsPerAmp);
//    /// Adds the given value to the list of sensitivities, and its gain counterpart to the list of gains.
//    void addSensitivityValue(double ampsPerVolt);

//    /// Adds the given units to the list of gain units, if they haven't already been added.
//    void addGainUnits(const QString &units);
//    /// Adds the given units to the list of sensitivity units, if they haven't already been added.
//    void addSensitivityUnits(const QString &units);

//    /// Clears the value lists associated with the gain.
//    void clearGainValues();
//    /// Clears the value lists associated with the sensitivity.
//    void clearSensitivityValues();

//    /// Clears the unit list associated with the gain.
//    void clearGainUnits();
//    /// Clears the unit list associated with the sensitivity.
//    void clearSensitivityUnits();

//    /// Returns the gain equivalent of the given sensitivity value and units, in gainUnits.
//    double gain(double sensitivityValue, const QString &sensitivityUnits, const QString &gainUnits);
//    /// Returns the sensitivity equivalent of the given gain value and units, in sensitivityUnits.
//    double sensitivity(double gainValue, const QString &gainUnits, const QString &sensitivityUnits);

//signals:
//    void gainValueAdded(double newValue);
//    void sensitivityValueAdded(double newValue);

//protected:
//    void setValues(double gain, double sensitivity);
//    bool contains(QList<double> list, double value);

//protected:
//    QList<double> gainValues_;
//    QList<double> sensitivityValues_;
//    QStringList gainValuesList_;
//    QStringList sensitivityValuesList_;
//    QStringList gainUnits_;
//    QStringList sensitivityUnits_;

//    int gainMaxIndex_;
//    int gainMinIndex_;
//    int sensitivityMaxIndex_;
//    int sensitivityMinIndex_;

//};

//#endif // CLSKEITHLEY428VALUEOPTIONS_H
