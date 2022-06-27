#include <QCoreApplication>
#include <QColor>
#include <QRgb>
#include <QMap>
#include <QStringList>
#include <QFileInfo>
#include <span>
#include <QVector>
#include <QImage>
#include <QDir>
#include <cmath>
#include <QTextStream>
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
typedef std::span<const QRgb> ConstPixelSpan;
typedef std::span<QRgb> PixelSpan;
typedef QMap<QString,QString> PathMap;
struct CultureDefinition {
	QString cultureMapPath;
	QColor cultureColour;
};
typedef QMap<QString,CultureDefinition> CultureMap;
static const QRgb YELLOW = QColor(255,255,0).rgb();
static const QRgb DARK_YELLOW = QColor(127,127,0).rgb();

bool cmp(const std::pair<QString, int>& a, const std::pair<QString, int>& b) {
	return a.second > b.second;
}
double compareWith(const ConstPixelSpan& mask, const ConstPixelSpan& pheno, const QColor& colour) {
	if(mask.size() != pheno.size()) return 0.0;
	QRgb maskColour = colour.rgb();
	double toReturn = 0.0;
	for(size_t i = 0; i < mask.size(); ++i) {
		const auto& maskC = mask[i];
		const auto& phenoC = pheno[i];
		if(maskC == maskColour) {
			if(phenoC == YELLOW) toReturn += 1.0;
			else if(phenoC == DARK_YELLOW) toReturn += 0.05;
		}
	}
	return toReturn;
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
	QTextStream DEBUG_OUT(stdout);
	for(auto it = std::begin(cultures); it != std::end(cultures); ++it) {
		try {
		DEBUG_OUT << "Processing " << it.key() << '\n';
		DEBUG_OUT.flush();
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
			DEBUG_OUT << e.what() << '\n';
			DEBUG_OUT.flush();
		}
	}
	DEBUG_OUT << "Finished!\n";
	DEBUG_OUT.flush();
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QTextStream DEBUG_OUT(stdout);
	QFile phenoDefinesFile(QStringLiteral("phenos.json"));
	QFile cultureDefinitionsFile(QStringLiteral("cultures.json"));
	if(!phenoDefinesFile.open(QFile::ReadOnly)) {
		DEBUG_OUT << "Did not find \'phenos.json\' in the application folder. Exiting!\n";
		DEBUG_OUT.flush();
		return 0;
	}
	if(!cultureDefinitionsFile.open(QFile::ReadOnly)) {
		DEBUG_OUT << "Did not find \'cultures.json\' in the application folder. Exiting!\n";
		DEBUG_OUT.flush();
		return 0;
	}
	PathMap phenotypes;
	CultureMap cultures;
	auto phenoDoc(QJsonDocument::fromJson(phenoDefinesFile.readAll()).object());
	auto cultureDoc(QJsonDocument::fromJson(cultureDefinitionsFile.readAll()).object());
	phenoDefinesFile.close();
	DEBUG_OUT << "Extracting phenotypes:\n\n";
	cultureDefinitionsFile.close();
	for(auto it = std::begin(phenoDoc); it != std::end(phenoDoc); ++it) {
		QString phenoPath = it.value().toString();
		DEBUG_OUT << it.key() << " - " << phenoPath << '\n';
		phenotypes.insert(it.key(),phenoPath);
	}
	DEBUG_OUT.flush();
	DEBUG_OUT << "Extracting cultures:\n\n";
	for(auto it = std::begin(cultureDoc); it != std::end(cultureDoc); ++it) {
		const auto obj = it.value().toObject();
		CultureDefinition culture;
		culture.cultureColour = obj["cultureColour"].toVariant().value<QColor>();
		culture.cultureMapPath = obj["cultureMapPath"].toString();
		DEBUG_OUT << it.key() << " - " << culture.cultureColour.name() << " - " << culture.cultureMapPath << '\n';
		cultures.insert(it.key(),culture);
	}
	DEBUG_OUT.flush();
	QDir outDir("autoassigned_phenotypes");
	if(!outDir.exists()) outDir.mkpath(".");
	doComparisons(cultures,phenotypes,outDir);
	return a.exec();
}
