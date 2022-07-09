#ifndef COMPARISON_HPP
#define COMPARISON_HPP
#include "Culture.hpp"
#include "Stdout.hpp"
#include <QDir>

void doTheComparisons(const ConstPixelSpan& mask, const QColor& colour, const PathMap& paths, QTextStream& textStream);
void doComparisons(const CultureMap& cultures, const PathMap& paths, const QDir& outputDirectory);

#endif // COMPARISON_HPP
