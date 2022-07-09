#ifndef COMPARISON_HPP
#define COMPARISON_HPP
#include "Culture.hpp"
#include "Stdout.hpp"
#include <QDir>


void doTheComparisons(const QMap<QString,double>& phenoOccurences, QTextStream& textStream);
void doTheComparisons(const ConstPixelSpan& mask, const QColor& colour, const PathMap& paths, QTextStream& textStream);
void doComparisons(const CultureMap& cultures, const PathMap& paths, const QDir& outputDirectory);
void doComparisons(const CultureMap& cultures, QJsonObject& cultureOut);
void doComparisons(const PathMap& paths, QJsonObject& phenoOut);

// PhenotypeIndexMap
void doTheComparisons(const IndexContainer& indices, const PhenotypeIndexMap& phenotypes, QTextStream& textStream);
void doComparisons(const CultureIndexMap& cultures, const PhenotypeIndexMap& phenotypes, const QDir& outputDirectory);
void outputCultureIndicesAsMaps(const CultureIndexMap& cultures, const QDir& outputDirectory);

#endif // COMPARISON_HPP
