#ifndef CULTURE_HPP
#define CULTURE_HPP
#include <QString>
#include <QColor>
#include <QMap>
#include <span>
#include <QVector>

struct CultureDefinition {
	QString cultureMapPath;
	QColor cultureColour;
};
typedef QMap<QString,QString> PathMap;
typedef QMap<QString,CultureDefinition> CultureMap;
typedef std::span<const QRgb> ConstPixelSpan;
typedef std::span<QRgb> PixelSpan;
void getCultureIndices(const ConstPixelSpan& mask, const QColor& colour, QVector<quint32>& indices);
void getPhenoIndices(const ConstPixelSpan& pheno, QVector<quint32>& primaryIndices);
void getPhenoIndices(const ConstPixelSpan& pheno, QVector<quint32>& primaryIndices, QVector<quint32>& secondaryIndices);
double compareWith(const ConstPixelSpan& mask, const ConstPixelSpan& pheno, const QColor& colour);

#endif // CULTURE_HPP
