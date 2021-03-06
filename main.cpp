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

CultureIndexMap getCultureIndices()
{
	CultureIndexMap cultures;
	QFile cultureDefinitionsFile(QStringLiteral("cultureIndices.json"));
	if(!cultureDefinitionsFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'cultureIndices.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return cultures;
	}
	auto cultureDoc(QJsonDocument::fromJson(cultureDefinitionsFile.readAll()).object());
	for(auto it = std::begin(cultureDoc); it != std::end(cultureDoc); ++it) {
		STDOUT << it.key() << '\n';
		if(it.value().isArray()) {
			QJsonArray arr = it.value().toArray();
			IndexContainer ind;
			for(const auto& zit : qAsConst(arr)) ind.insert(zit.toInt());
			cultures.insert(it.key(),ind);
		}
	}
	return cultures;
}
PhenotypeIndexMap getPhenotypeIndices()
{
	PhenotypeIndexMap phenotypes;
	QFile phenoDefinesFile(QStringLiteral("phenotypeIndices.json"));
	if(!phenoDefinesFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'phenotypeIndices.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return phenotypes;
	}
	auto phenoDoc(QJsonDocument::fromJson(phenoDefinesFile.readAll()).object());
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
	return phenotypes;
}

void useIndices()
{
	auto cultures = getCultureIndices();
	auto phenotypes = getPhenotypeIndices();
	QDir outDir("autoassigned_phenotypes");
	if(!outDir.exists()) outDir.mkpath(".");
	doComparisons(cultures,phenotypes,outDir);
}

void generateCultureMaps()
{
	auto cultures = getCultureIndices();
	QDir outDir("culture_maps");
	if(!outDir.exists()) outDir.mkpath(".");
	outputCultureIndicesAsMaps(cultures,outDir);
}
void optimizedOldWay() {
	if(readBasicPhenotypes()) {
		PhenotypeIndexMap nphenotypes;
		CultureIndexMap ncultures;
		doComparisons(phenotypes,nphenotypes);
		doComparisons(cultures,ncultures);
		QDir outDir("autoassigned_phenotypes");
		if(!outDir.exists()) outDir.mkpath(".");
		doComparisons(ncultures,nphenotypes,outDir);
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QStringList args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(QString::fromLocal8Bit(argv[i]));
	}
	if(args.size() < 2 || args[1] == QStringLiteral("--help")) {
		STDOUT << "Correct usage:\n"
			   << args[0] << " --help                - Brings up this instruction guide\n"
			   << args[0] << " --createIndices       - Creates phenotype and culture indices for future usage.\n"
			   << args[0] << " --generateCultureMaps - Generates new culture maps from said indices.\n"
			   << args[0] << " --useIndices          - Uses said indices to generate phenotype assignments for cultures.\n"
			   << args[0] << " --oldWay              - Generates phenotype assignments for cultures the old-fashioend, ineffective way. (DEPRECATED, NOT RECOMMENDED!)\n"
			   << args[0] << " --optimizedOldWay     - A more optimized version of the old way\n";
		STDOUT.flush();
	}
	else if(args[1] == QStringLiteral("--createIndices")) outputIndices();
	else if(args[1] == QStringLiteral("--useIndices")) useIndices();
	else if(args[1] == QStringLiteral("--generateCultureMaps")) generateCultureMaps();
	else if(args[1] == QStringLiteral("--oldWay")) defaultBehaviour();
	else if(args[1] == QStringLiteral("--optimizedOldWay")) optimizedOldWay();
	else {
		STDOUT << "Invalid command!\n";
		STDOUT.flush();
	}
	return 0;
}
