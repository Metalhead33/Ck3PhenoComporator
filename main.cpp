#include <QCoreApplication>
#include "Culture.hpp"
#include "Stdout.hpp"
#include "Comparison.hpp"
#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

QTextStream STDOUT(stdout);

void defaultBehaviour() {
	QFile phenoDefinesFile(QStringLiteral("phenos.json"));
	QFile cultureDefinitionsFile(QStringLiteral("cultures.json"));
	if(!phenoDefinesFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'phenos.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return;
	}
	if(!cultureDefinitionsFile.open(QFile::ReadOnly)) {
		STDOUT << "Did not find \'cultures.json\' in the application folder. Exiting!\n";
		STDOUT.flush();
		return;
	}
	PathMap phenotypes;
	CultureMap cultures;
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
	QDir outDir("autoassigned_phenotypes");
	if(!outDir.exists()) outDir.mkpath(".");
	doComparisons(cultures,phenotypes,outDir);
}


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	defaultBehaviour();
	return 0;
}
