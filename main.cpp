#include <QCoreApplication>
#include "Culture.hpp"
#include "Stdout.hpp"
#include "Comparison.hpp"
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>

QTextStream STDOUT(stdout);
static PathMap phenotypes;
static PathMap traits;
static CultureMap cultures;

bool readBasicPhenotypes() {
	QFile phenoDefinesFile(QStringLiteral("phenos.json"));
	QFile cultureDefinitionsFile(QStringLiteral("cultures.json"));
	if(!phenoDefinesFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'phenos.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return false;
	}
	if(!cultureDefinitionsFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'cultures.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return false;
	}
	auto phenoDoc(QJsonDocument::fromJson(phenoDefinesFile.readAll()).object());
	auto cultureDoc(QJsonDocument::fromJson(cultureDefinitionsFile.readAll()).object());
	phenoDefinesFile.close();
	STDOUT << "Extracting phenotypes:\n\n";
	cultureDefinitionsFile.close();
	for(auto it = std::begin(phenoDoc); it != std::end(phenoDoc); ++it) {
		QString phenoPath = it.value().toString();
		STDOUT << it.key() << " - " << phenoPath << '\n';
		phenotypes.insert(it.key(),phenoPath);
	}
	STDOUT.flush();
	STDOUT << "Extracting cultures:\n\n";
	for(auto it = std::begin(cultureDoc); it != std::end(cultureDoc); ++it) {
		const auto obj = it.value().toObject();
		CultureDefinition culture;
		culture.cultureColour = obj["cultureColour"].toVariant().value<QColor>();
		culture.cultureMapPath = obj["cultureMapPath"].toString();
		STDOUT << it.key() << " - " << culture.cultureColour.name() << " - " << culture.cultureMapPath << '\n';
		cultures.insert(it.key(),culture);
	}
	STDOUT.flush();
	return true;
}
void defaultBehaviour() {
	if(readBasicPhenotypes()) {
		QDir outDir("autoassigned_phenotypes");
		if(!outDir.exists()) outDir.mkpath(".");
		doComparisons(cultures,phenotypes,outDir);
	}
}
void outputIndices() {
	if(!readBasicPhenotypes()) return;
	QFile traitsFIle(QStringLiteral("traits.json"));
	if(traitsFIle.open(QFile::ReadOnly)) {
		auto phenoDoc(QJsonDocument::fromJson(traitsFIle.readAll()).object());
		for(auto it = std::begin(phenoDoc); it != std::end(phenoDoc); ++it) {
			QString phenoPath = it.value().toString();
			STDOUT << it.key() << " - " << phenoPath << '\n';
			traits.insert(it.key(),phenoPath);
		}
	} else {
		STDOUT << "Did not find \'traits.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return;
	}
	QFile cultureJson(QStringLiteral("cultureIndices.json"));
	QFile phenotypeJson(QStringLiteral("phenotypeIndices.json"));
	QFile traitJson(QStringLiteral("traitIndices.json"));
	if( !(cultureJson.open(QFile::WriteOnly) && phenotypeJson.open(QFile::WriteOnly) && traitJson.open(QFile::WriteOnly)) ) return;
	QJsonObject ncultures,nphenotypes,ntraits;
	doComparisons(cultures,ncultures);
	doComparisons(phenotypes,nphenotypes);
	doComparisons(traits,ntraits);
	cultureJson.write(QJsonDocument(ncultures).toJson());
	cultureJson.flush();
	cultureJson.close();
	phenotypeJson.write(QJsonDocument(nphenotypes).toJson());
	phenotypeJson.flush();
	phenotypeJson.close();
	traitJson.write(QJsonDocument(ntraits).toJson());
	traitJson.flush();
	traitJson.close();
}
void useIndices()
{
	QFile phenoDefinesFile(QStringLiteral("phenotypeIndices.json"));
	QFile cultureDefinitionsFile(QStringLiteral("cultureIndices.json"));
	if(!phenoDefinesFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'phenotypeIndices.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return;
	}
	if(!cultureDefinitionsFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'cultureIndices.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return;
	}
	auto phenoDoc(QJsonDocument::fromJson(phenoDefinesFile.readAll()).object());
	auto cultureDoc(QJsonDocument::fromJson(cultureDefinitionsFile.readAll()).object());
	CultureIndexMap cultures;
	PhenotypeIndexMap phenotypes;
	for(auto it = std::begin(phenoDoc); it != std::end(phenoDoc); ++it) {
		STDOUT << it.key() << '\n';
		if(it.value().isObject()) {
			QJsonObject obj = it.value().toObject();
			PhenotypeIndexContianer ind;
			QJsonArray prim = obj["primary"].toArray();
			QJsonArray secondary = obj["secondary"].toArray();
			for(const auto& zit : qAsConst(prim)) ind.primary.insert(zit.toInt());
			for(const auto& zit : qAsConst(secondary)) ind.secondary.insert(zit.toInt());
			phenotypes.insert(it.key(),ind);
		}
	}
	for(auto it = std::begin(cultureDoc); it != std::end(cultureDoc); ++it) {
		STDOUT << it.key() << '\n';
		if(it.value().isArray()) {
			QJsonArray arr = it.value().toArray();
			IndexContainer ind;
			for(const auto& zit : qAsConst(arr)) ind.insert(zit.toInt());
			cultures.insert(it.key(),ind);
		}
	}
	QDir outDir("autoassigned_phenotypes");
	if(!outDir.exists()) outDir.mkpath(".");
	doComparisons(cultures,phenotypes,outDir);
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QStringList args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(QString::fromLocal8Bit(argv[i]));
	}
	if(args.size() >= 2 && args[1] == QStringLiteral("--createIndices")) outputIndices();
	else if(args.size() >= 2 && args[1] == QStringLiteral("--useIndices")) useIndices();
	else defaultBehaviour();
	return 0;
}
