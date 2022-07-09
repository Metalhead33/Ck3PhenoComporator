#ifndef CULTURE_HPP
#define CULTURE_HPP
#include <QString>
#include <QColor>
#include <QMap>
#include <span>
#include <QVector>
#include <QSet>

struct CultureDefinition {
	QString cultureMapPath;
	QColor cultureColour;
};
typedef QSet<int> IndexContainer;
struct PhenotypeIndexContianer {
	QSet<int> primary;
	QSet<int> secondary;
};
typedef QMap<QString,IndexContainer> CultureIndexMap;
typedef QMap<QString,PhenotypeIndexContianer> PhenotypeIndexMap;
typedef QMap<QString,QString> PathMap;
typedef QMap<QString,CultureDefinition> CultureMap;
typedef std::span<const QRgb> ConstPixelSpan;
typedef std::span<QRgb> PixelSpan;
void getCultureIndices(const ConstPixelSpan& mask, const QColor& colour, IndexContainer& indices);
void getPhenoIndices(const ConstPixelSpan& pheno, IndexContainer primaryIndices);
void getPhenoIndices(const ConstPixelSpan& pheno, IndexContainer& primaryIndices, IndexContainer& secondaryIndices);
void getPhenoIndices(const ConstPixelSpan& pheno, PhenotypeIndexContianer& container);
double compareWith(const ConstPixelSpan& mask, const ConstPixelSpan& pheno, const QColor& colour);
double compareWith(const IndexContainer& cultureIndices, const IndexContainer& phenoIndices, bool secondary = false);

#endif // CULTURE_HPP
