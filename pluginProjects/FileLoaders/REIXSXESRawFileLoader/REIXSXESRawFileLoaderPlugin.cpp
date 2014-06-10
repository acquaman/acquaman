#include "REIXSXESRawFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include "util/AMErrorMonitor.h"

#include <QPair>

/// Magic string found and the beginning and end of the file to identify the file type and version
#define REIXSXESRAWFILELOADER_MAGIC_STRING "REIXSXESRAW2011.1"

typedef QPair<int, QString> REIXSXESRawFileException;


#include "dataman/AMScan.h"

bool REIXSXESRawFileLoaderPlugin::accepts(AMScan *scan){
	return (scan->fileFormat() == "reixsXESRaw");
}

bool REIXSXESRawFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){

	if(!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative()){
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());
	}

	QFile file(sourceFileInfo.filePath());
	qint32 pixelsX, pixelsY;
	qint32* imageArray = 0;
	QList<AMAxisInfo> detectorAxes;

	try {

		if(!file.open(QIODevice::ReadOnly))
			throw REIXSXESRawFileException(REIXSXESRAWFILELOADERPLUGIN_CANNOT_OPEN_FILE, "Could not open the data file");


		// check the magic string (ascii encoded string at the start of the file)
		QByteArray magicTextShouldBe(REIXSXESRAWFILELOADER_MAGIC_STRING);
		QByteArray magicText = file.read(magicTextShouldBe.length());
		if(magicText != magicTextShouldBe)
			throw REIXSXESRawFileException(-2, QString("Couldn't load invalid file format. Should read '%1', read '%2' instead").arg((QString::fromAscii(magicTextShouldBe.constData()))).arg(QString::fromAscii(magicText.constData())));


		QDataStream ds(&file);
		// read number of pixels in X and Y
		ds >> pixelsX;
		ds >> pixelsY;

		if(pixelsX == 0 || pixelsY == 0)
			throw REIXSXESRawFileException(-3, "Couldn't load corrupted file");

		// read all the pixel values
		imageArray = new qint32[pixelsX*pixelsY];
		qint32* writePointer = imageArray;
		for(int i=0, cc=pixelsX*pixelsY; i<cc; i++)
			ds >> *(writePointer++);

		// read the magic string at the end of the file (makes sure we had all the data)
		magicText = file.read(magicTextShouldBe.length());
		if(magicText != magicTextShouldBe)
			throw REIXSXESRawFileException(-2, "Couldn't load corrupted file -- there wasn't enough data.");

		file.close();
	}
	catch(REIXSXESRawFileException e) {
		errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Alert, e.first, QString("REIXS XES Raw File Loader: %1\n\nFile: '%2'").arg(e.second).arg(sourceFileInfo.filePath())));
		file.close();
		if(imageArray)
			delete [] imageArray;
		return false;
	}

	scan->clearRawDataCompletely();

	detectorAxes.append(AMAxisInfo("x", pixelsX, "x - energy axis", "pixels"));
	detectorAxes.append(AMAxisInfo("y", pixelsY, "y - vertical axis", "pixels"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("xesImage",
													  "XES Detector Image",
													  "counts",
													  detectorAxes));

	if(!scan->rawData()->setValue(AMnDIndex(), 0, imageArray))
		errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Alert, REIXSXESRAWFILELOADERPLUGIN_CANNOT_SET_IMAGE_VALUE, "Could not set detector image value. Please report this bug to the acquaman developers."));

	delete [] imageArray;



	// There's pre-existing raw data sources. make sure that there's only one raw data source: (integrity check: we only have one measurement in the raw data)
	while(scan->rawDataSources()->count() > 1) {
		int i = scan->rawDataSources()->count()-1;
		errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, REIXSXESRAWFILELOADERPLUGIN_DATA_COLUMN_MISMATCH, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
		scan->deleteRawDataSource(i);
	}


	return true;
}

bool REIXSXESRawFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "reixsXESRaw");
}

Q_EXPORT_PLUGIN2(REIXSXESRawFileLoaderFactory, REIXSXESRawFileLoaderFactory)

