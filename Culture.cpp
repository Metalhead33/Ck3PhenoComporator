#include "Culture.hpp"

static const QRgb YELLOW = QColor(255,255,0).rgb();
static const QRgb DARK_YELLOW = QColor(127,127,0).rgb();
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

void getCultureIndices(const ConstPixelSpan& mask, const QColor& colour, QVector<int>& indices)
{
	QRgb maskColour = colour.rgb();
	for(size_t i = 0; i < mask.size(); ++i) {
		const auto& maskC = mask[i];
		if(maskC == maskColour) {
			indices.push_back(i);
		}
	}
}

void getPhenoIndices(const ConstPixelSpan& pheno, QVector<int>& primaryIndices, QVector<int>& secondaryIndices)
{
	for(size_t i = 0; i < pheno.size(); ++i) {
		const auto& phenoC = pheno[i];
		if(phenoC == YELLOW) primaryIndices.push_back(i);
		else if(phenoC == DARK_YELLOW) secondaryIndices.push_back(i);
	}
}

void getPhenoIndices(const ConstPixelSpan& pheno, QVector<int>& primaryIndices)
{
	for(size_t i = 0; i < pheno.size(); ++i) {
		const auto& phenoC = pheno[i];
		if(phenoC == YELLOW) primaryIndices.push_back(i);
	}
}

double compareWith(const IndexContainer& cultureIndices, const IndexContainer& phenoIndices, bool secondary)
{
	IndexContainer nindices = cultureIndices;
	nindices.intersect(phenoIndices);
	if(secondary) return static_cast<double>(nindices.size()) * 0.05;
	else return static_cast<double>(nindices.size());
}
