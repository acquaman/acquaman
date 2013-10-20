#ifndef SGMXASSCANACTIONCONTROLLERFILEWRITER_H
#define SGMXASSCANACTIONCONTROLLERFILEWRITER_H

#include <QString>
#include <QObject>

class QFile;

class SGMXASScanActionControllerFileWriter : public QObject
{
Q_OBJECT
public:
	enum FileWriterError{
		AlreadyExistsError = 0,		///< The file passed in already exists
		CouldNotOpenError = 1,		///< The file could not be opened
		InvalidError = 2
	};

	SGMXASScanActionControllerFileWriter(const QString &filePath, bool hasRank2Data = false, QObject *parent = 0);

public slots:
	void writeToFile(int fileRank, const QString &textToWrite);
	void finishWriting();

signals:
	void fileWriterError(SGMXASScanActionControllerFileWriter::FileWriterError error);
	void fileWriterIsBusy(bool isBusy);

protected slots:
	void emitError(SGMXASScanActionControllerFileWriter::FileWriterError error);
	void emitErrors();
	void emitFileWriterIsBusy();

protected:
	QString filePath_;
	bool hasRank2Data_;

	QFile *rank1File_;
	QFile *rank2File_;

	QList<SGMXASScanActionControllerFileWriter::FileWriterError> errorsList_;
};

//Q_DECLARE_METATYPE(SGMXASScanActionControllerFileWriter::FileWriterError)

#endif // SGMXASSCANACTIONCONTROLLERFILEWRITER_H
