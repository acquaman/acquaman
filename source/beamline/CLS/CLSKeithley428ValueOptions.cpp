//#include "CLSKeithley428ValueOptions.h"

//CLSKeithley428ValueOptions::CLSKeithley428ValueOptions(QObject *parent) :
//    QObject(parent)
//{
//    gainMaxIndex_ = 0;
//    gainMinIndex_ = 0;

//    sensitivityMaxIndex_ = 0;
//    sensitivityMinIndex_ = 0;
//}

//CLSKeithley428ValueOptions::~CLSKeithley428ValueOptions()
//{

//}

//QList<double> CLSKeithley428ValueOptions::gainValues() const
//{
//    return gainValues_;
//}

//QStringList CLSKeithley428ValueOptions::gainList() const
//{
//    return gainValuesList_;
//}

//QStringList CLSKeithley428ValueOptions::gainUnits() const
//{
//    return gainUnits_;
//}

//QList<double> CLSKeithley428ValueOptions::sensitivityValues() const
//{
//    return sensitivityValues_;
//}

//QStringList CLSKeithley428ValueOptions::sensitivityList() const
//{
//    return sensitivityValuesList_;
//}

//QStringList CLSKeithley428ValueOptions::sensitivityUnits() const
//{
//    return sensitivityUnits_;
//}

//double CLSKeithley428ValueOptions::gainValueAt(int index)
//{
//    if (gainValueIndexOkay(index))
//        return gainValues_.at(index);

//    qDebug() << "Index out of range.";
//    return -1;
//}

//QString CLSKeithley428ValueOptions::gainUnitsAt(int index) const
//{
//    if (gainUnitIndexOkay(index)) {
//        return gainUnits_.at(index);
//    }

//    qDebug() << "Index out of range.";
//    return "";
//}

//double CLSKeithley428ValueOptions::sensitivityValueAt(int index)
//{
//    if (sensitivityValueIndexOkay(index)) {
//        return sensitivityValues_.at(index);
//    }

//    qDebug() << "Index out of range.";
//    return -1;
//}

//QString CLSKeithley428ValueOptions::sensitivityUnitsAt(int index) const
//{
//    if (sensitivityUnitIndexOkay(index)) {
//        return sensitivityUnits_.at(index);
//    }

//    qDebug() << "Index out of range.";
//    return "";
//}

//double CLSKeithley428ValueOptions::gainValueMax()
//{
//    return gainValueAt(gainMaxIndex_);
//}

//double CLSKeithley428ValueOptions::gainValueMin()
//{
//    return gainValueAt(gainMinIndex_);
//}

//double CLSKeithley428ValueOptions::sensitivityValueMax()
//{
//    return sensitivityValueAt(sensitivityMaxIndex_);
//}

//double CLSKeithley428ValueOptions::sensitivityValueMin()
//{
//    return sensitivityValueAt(sensitivityMinIndex_);
//}

//int CLSKeithley428ValueOptions::gainValueIndexMax()
//{
//    return gainMaxIndex_;
//}

//int CLSKeithley428ValueOptions::gainValueIndexMin()
//{
//    return gainMinIndex_;
//}

//bool CLSKeithley428ValueOptions::gainValueIndexOkay(int index)
//{
//    return (index >= 0 && index < gainValues_.size());
//}

//bool CLSKeithley428ValueOptions::sensitivityValueIndexOkay(int index)
//{
//    return (index >= 0 && index < sensitivityValues_.size());
//}

//bool CLSKeithley428ValueOptions::gainUnitIndexOkay(int index) const
//{
//    return (index >= 0 && index < gainUnits_.size());
//}

//bool CLSKeithley428ValueOptions::sensitivityUnitIndexOkay(int index) const
//{
//    return (index >= 0 && index < sensitivityUnits_.size());
//}

//double CLSKeithley428ValueOptions::gain(double sensitivityVal, const QString &sensitivityUnits, const QString &gainUnits)
//{
//    double voltsPerAmp = -1;

//    if (sensitivityUnits == "A/V") {
//        voltsPerAmp = 1/sensitivityVal;
//    } else if (sensitivityUnits == "mA/V") {
//        voltsPerAmp = 1/sensitivityVal * pow(10, 3);
//    } else if (sensitivityUnits == "uA/V") {
//        voltsPerAmp = 1/sensitivityVal * pow(10, 6);
//    } else if (sensitivityUnits == "nA/V") {
//        voltsPerAmp = 1/sensitivityVal * pow(10, 9);
//    } else if (sensitivityUnits == "pA/V") {
//        voltsPerAmp = 1/sensitivityVal * pow(10, 12);
//    } else {
//        qDebug() << "Units given were not recognized. Conversion failed.";
//    }

//    double gain = -1;

//    if (gainUnits == "V/A") {
//        gain = voltsPerAmp;
//    } else {
//        qDebug() << "Gain units given were not recognized. Conversion failed.";
//    }

//    return gain;
//}

//double CLSKeithley428ValueOptions::sensitivity(double gainValue, const QString &gainUnits, const QString &sensitivityUnits)
//{
//    double voltsPerAmp = -1;

//    if (gainUnits == "V/A") {
//        voltsPerAmp = gainValue;
//    } else {
//        qDebug() << "Gain units given were not recognized. Conversion failed.";
//    }

//    double sensitivity = -1;

//    if (sensitivityUnits == "A/V") {
//        sensitivity = 1/voltsPerAmp;
//    } else if (sensitivityUnits == "mA/V") {
//        sensitivity = 1/voltsPerAmp * pow(10, -3);
//    } else if (sensitivityUnits == "uA/V") {
//        sensitivity = 1/voltsPerAmp * pow(10, -6);
//    } else if (sensitivityUnits == "nA/V") {
//        sensitivity = 1/voltsPerAmp * pow(10, -9);
//    } else if (sensitivityUnits == "pA/V") {
//        sensitivity = 1/voltsPerAmp * pow(10, -12);
//    } else {
//        qDebug() << "Units given were not recognized. Conversion failed.";
//    }

//    return sensitivity;
//}

//void CLSKeithley428ValueOptions::clearGainValues()
//{
//    gainValues_.clear();
//    gainValuesList_.clear();
//}

//void CLSKeithley428ValueOptions::clearSensitivityValues()
//{
//    sensitivityValues_.clear();
//    sensitivityValuesList_.clear();
//}

//void CLSKeithley428ValueOptions::clearGainUnits()
//{
//    gainUnits_.clear();
//}

//void CLSKeithley428ValueOptions::clearSensitivityUnits()
//{
//    sensitivityUnits_.clear();
//}

//void CLSKeithley428ValueOptions::addGainValue(double voltsPerAmp)
//{
//    qDebug() << "Gain value to add : " << voltsPerAmp;
//    qDebug() << "Gain values : " << gainValues_;

////    double val = pow(10, 5);

////    if (!contains(gainValues_, val)) {
////        qDebug() << val << " is being added for the first time.";
////    }

//    if (!contains(gainValues_, voltsPerAmp)) {

//        // add the value to the list of gain values.
//        gainValues_.append(voltsPerAmp);
//        gainValuesList_.append(QString("%1").arg(voltsPerAmp, 0, 'e', 2));

//        qDebug() << "Added gain value : " << voltsPerAmp;

//        // add the sensitivity value to the list of sensitivities.
//        double sensitivityVal = sensitivity(voltsPerAmp, "V/A", "A/V");

////        if (!sensitivityValues_.contains(sensitivityVal))
////            addSensitivityValue(sensitivityVal);

//        emit gainValueAdded(voltsPerAmp);
//    }
//}

//void CLSKeithley428ValueOptions::addSensitivityValue(double ampsPerVolt)
//{
//    qDebug() << "Sensitivity value to add : " << ampsPerVolt;

//    if (!sensitivityValues_.contains(ampsPerVolt)) {

//        // add the value to the list of sensitivity values.
//        sensitivityValues_.append(ampsPerVolt);
//        sensitivityValuesList_.append(QString("%1").arg(ampsPerVolt, 0, 'e', 2));

//        qDebug() << "Added sensitivity value : " << ampsPerVolt;

//        // add the corresponding gain to the list of gains.
//        double gainVal = gain(ampsPerVolt, "A/V", "V/A");

//        if (!gainValues_.contains(gainVal)) {
//            addGainValue(gainVal);
//        }

//        emit sensitivityValueAdded(ampsPerVolt);
//    }
//}

//void CLSKeithley428ValueOptions::addGainUnits(const QString &units)
//{
//    if (!gainUnits_.contains(units)) {
//        gainUnits_.append(units);
//    }
//}

//void CLSKeithley428ValueOptions::addSensitivityUnits(const QString &units)
//{
//    if (!sensitivityUnits_.contains(units)) {
//        sensitivityUnits_.append(units);
//    }
//}

//bool CLSKeithley428ValueOptions::contains(QList<double> list, double value)
//{
//    bool valueFound = false;

//    foreach (double val, list) {
//        if (val == value) {
//            valueFound = true;
//            break;
//        }
//    }

//    return valueFound;
//}


