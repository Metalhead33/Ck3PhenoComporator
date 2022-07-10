#include "Comparison.hpp"
#include <QStringList>
#include <QFileInfo>
#include <QVector>
#include <QImage>
#include <cmath>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

static bool cmp(const std::pair<QString, int>& a, const std::pair<QString, int>& b) {
	return a.second > b.second;
}
void doTheComparisons(const QMap<QString,double>& phenoOccurences, QTextStream& textStream) {
	double summa = 0.0;
	double greatest = 0.0;
	for(auto it = std::begin(phenoOccurences); it != std::end(phenoOccurences); ++it) {
		summa += it.value();
		greatest = std::max(greatest,it.value());
	}
	//summa = 1.0/summa;
	summa = 1.0/greatest;
	QVector<std::pair<QString,int>> intMap;
	for(auto it = std::begin(phenoOccurences); it != std::end(phenoOccurences); ++it) {
		if(it.value() >= 0.1) intMap.push_back(std::make_pair(it.key(),int(std::ceil((it.value()*summa)*100.0))) );
	}
	sort(intMap.begin(), intMap.end(), cmp);
	for(const auto& it : intMap) {
		STDOUT << "\t\t\t" << it.second << " = " << it.first << "\n";
		textStream << "\t\t\t" << it.second << " = " << it.first << "\n";
	}
	textStream.flush();
	STDOUT.flush();
}
void doTheComparisons(const ConstPixelSpan& mask, const QColor& colour, const PathMap& paths, QTextStream& textStream) {
	QMap<QString,double> phenoOccurences;
	for(auto it = std::begin(paths); it != std::end(paths); ++it) {
		QFileInfo fileinfo(it.value());
		QImage imag(fileinfo.absoluteFilePath());
		if(imag.format() != QImage::Format_RGB32) imag.convertTo(QImage::Format_RGB32);
		phenoOccurences.insert(it.key(),compareWith(mask, ConstPixelSpan(reinterpret_cast<const QRgb*>(imag.bits()),static_cast<size_t>( imag.width() * imag.height()) ),colour));
	}
	doTheComparisons(phenoOccurences,textStream);
}

void doComparisons(const CultureMap& cultures, const PathMap& paths, const QDir& outputDirectory) {
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		try {
			STDOUT << "Processing " << it.key() << '\n';
			STDOUT.flush();
			QImage cultureImg(it.value().cultureMapPath);
			if(cultureImg.format() != QImage::Format_RGB32) cultureImg.convertTo(QImage::Format_RGB32);
			ConstPixelSpan cultureMap(reinterpret_cast<const QRgb*>(cultureImg.bits()),static_cast<size_t>( cultureImg.width() * cultureImg.height() ) );
			QFile fileOut(outputDirectory.absoluteFilePath(QStringLiteral("%1.txt").arg(it.key())));
			if(fileOut.open(QFile::WriteOnly | QFile::Text)) {
				QTextStream textStream(&fileOut);
				doTheComparisons(cultureMap,it.value().cultureColour,paths,textStream);
			}
		}  catch (std::exception& e) {
			STDOUT << e.what() << '\n';
			STDOUT.flush();
		}
	}
	STDOUT << "Finished!\n";
	STDOUT.flush();
}

void doComparisons(const PathMap& paths, QJsonObject& phenoOut)
{
	for(auto it = std::begin(paths); it != std::end(paths); ++it) {
		QFileInfo fileinfo(it.value());
		QImage imag(fileinfo.absoluteFilePath());
		if(imag.format() != QImage::Format_RGB32) imag.convertTo(QImage::Format_RGB32);
		ConstPixelSpan phenotypeMap(reinterpret_cast<const QRgb*>(imag.bits()),static_cast<size_t>(imag.width() * imag.height()) );
		IndexContainer primaryIndices;
		IndexContainer secondaryIndices;
		getPhenoIndices(phenotypeMap,primaryIndices,secondaryIndices);
		QJsonArray primaryArr,secondaryArr;
		for(const auto& zit : qAsConst(primaryIndices)) primaryArr.push_back(zit);
		for(const auto& zit : qAsConst(secondaryIndices)) secondaryArr.push_back(zit);
		QJsonObject tmpObj;
		tmpObj["primary"] = primaryArr;
		tmpObj["secondary"] = primaryArr;
		phenoOut[it.key()] = tmpObj;
	}
}
void doComparisons(const PathMap& paths, PhenotypeIndexMap& phenoOut)
{
	for(auto it = std::begin(paths); it != std::end(paths); ++it) {
		QFileInfo fileinfo(it.value());
		QImage imag(fileinfo.absoluteFilePath());
		if(imag.format() != QImage::Format_RGB32) imag.convertTo(QImage::Format_RGB32);
		ConstPixelSpan phenotypeMap(reinterpret_cast<const QRgb*>(imag.bits()),static_cast<size_t>(imag.width() * imag.height()) );
		PhenotypeIndexContianer indcont;
		getPhenoIndices(phenotypeMap,indcont.primary,indcont.secondary);
		phenoOut.insert(it.key(),indcont);
	}
}

void doComparisons(const CultureMap& cultures, QJsonObject& cultureOut)
{
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		QImage cultureImg(it.value().cultureMapPath);
		if(cultureImg.format() != QImage::Format_RGB32) cultureImg.convertTo(QImage::Format_RGB32);
		ConstPixelSpan cultureMap(reinterpret_cast<const QRgb*>(cultureImg.bits()),static_cast<size_t>( cultureImg.width() * cultureImg.height() ) );
		IndexContainer indices;
		getCultureIndices(cultureMap,it.value().cultureColour,indices);
		QJsonArray arr;
		for(const auto& zit : qAsConst(indices)) arr.push_back(zit);
		cultureOut[it.key()] = arr;
	}
}

void doComparisons(const CultureMap& cultures, CultureIndexMap& cultureOut)
{
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		QImage cultureImg(it.value().cultureMapPath);
		if(cultureImg.format() != QImage::Format_RGB32) cultureImg.convertTo(QImage::Format_RGB32);
		ConstPixelSpan cultureMap(reinterpret_cast<const QRgb*>(cultureImg.bits()),static_cast<size_t>( cultureImg.width() * cultureImg.height() ) );
		IndexContainer indices;
		getCultureIndices(cultureMap,it.value().cultureColour,indices);
		cultureOut.insert(it.key(),indices);
	}
}

void doTheComparisons(const IndexContainer& indices, const PhenotypeIndexMap& phenotypes, QTextStream& textStream)
{
	QMap<QString,double> phenoOccurences;
	for(auto it = std::begin(phenotypes); it != std::end(phenotypes); ++it) {
		double val = 0.0;
		val += compareWith(indices,it.value().primary,false);
		val += compareWith(indices,it.value().secondary,true);
		phenoOccurences.insert(it.key(),val);
	}
	doTheComparisons(phenoOccurences,textStream);
}

void doComparisons(const CultureIndexMap& cultures, const PhenotypeIndexMap& phenotypes, const QDir& outputDirectory)
{
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		try {
			STDOUT << "Processing " << it.key() << '\n';
			STDOUT.flush();
			QFile fileOut(outputDirectory.absoluteFilePath(QStringLiteral("%1.txt").arg(it.key())));
			if(fileOut.open(QFile::WriteOnly | QFile::Text)) {
				QTextStream textStream(&fileOut);
				doTheComparisons(it.value(),phenotypes,textStream);
			}
		}  catch (std::exception& e) {
			STDOUT << e.what() << '\n';
			STDOUT.flush();
		}
	}
	STDOUT << "Finished!\n";
	STDOUT.flush();
}

void outputCultureIndicesAsMaps(const CultureIndexMap& cultures, const QDir& outputDirectory)
{
	try {
		QImage imag(":/root/plainMap.png");
		QColor yellow(255,255,0);
		if(imag.format() != QImage::Format_RGB32) imag.convertTo(QImage::Format_RGB32);
		for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
			STDOUT << "Processing " << it.key() << '\n';
			STDOUT.flush();
			QString outPath = outputDirectory.absoluteFilePath(QStringLiteral("%1.png").arg(it.key()));
			QImage pulsats = imag.copy();
			for(const auto& zit : it.value()) {
				int x = zit % pulsats.width();
				int y = zit / pulsats.width();
				pulsats.setPixelColor(x,y,yellow);
			}
			pulsats.save(outPath);
		}
	} catch (std::exception& e) {
		STDOUT << e.what() << '\n';
		STDOUT.flush();
	}
}
