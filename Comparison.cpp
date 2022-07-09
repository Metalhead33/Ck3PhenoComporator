#include "Comparison.hpp"
#include <QStringList>
#include <QFileInfo>
#include <QVector>
#include <QImage>
#include <cmath>
#include <QFile>

static bool cmp(const std::pair<QString, int>& a, const std::pair<QString, int>& b) {
	return a.second > b.second;
}
void doTheComparisons(const ConstPixelSpan& mask, const QColor& colour, const PathMap& paths, QTextStream& textStream) {
	QMap<QString,double> phenoOccurences;
	for(auto it = std::begin(paths); it != std::end(paths); ++it) {
		QFileInfo fileinfo(it.value());
		QImage imag(fileinfo.absoluteFilePath());
		if(imag.format() != QImage::Format_RGB32) imag.convertTo(QImage::Format_RGB32);
		phenoOccurences.insert(it.key(),compareWith(mask, ConstPixelSpan(reinterpret_cast<const QRgb*>(imag.bits()),imag.width() * imag.height()),colour));
	}
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
		textStream << "\t\t\t" << it.second << " = " << it.first << "\n";
	}
}

void doComparisons(const CultureMap& cultures, const PathMap& paths, const QDir& outputDirectory) {
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		try {
			STDOUT << "Processing " << it.key() << '\n';
			STDOUT.flush();
			QImage cultureImg(it.value().cultureMapPath);
			if(cultureImg.format() != QImage::Format_RGB32) cultureImg.convertTo(QImage::Format_RGB32);
			ConstPixelSpan cultureMap(reinterpret_cast<const QRgb*>(cultureImg.bits()),cultureImg.width() * cultureImg.height());
			QFile fileOut(outputDirectory.absoluteFilePath(QStringLiteral("%1.txt").arg(it.key())));
			if(fileOut.open(QFile::WriteOnly | QFile::Text)) {
				QTextStream textStream(&fileOut);
				doTheComparisons(cultureMap,it.value().cultureColour,paths,textStream);
				textStream.flush();
				fileOut.flush();
				fileOut.close();
			}
		}  catch (std::exception& e) {
			STDOUT << e.what() << '\n';
			STDOUT.flush();
		}
	}
	STDOUT << "Finished!\n";
	STDOUT.flush();
}
