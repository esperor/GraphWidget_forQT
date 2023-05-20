#pragma once

#include <QColor>
#include <QSize>

#include "GraphLib_global.h"

namespace GraphLib {

// --------- COMMON ---------
// COMMON RENDER CONSTANTS

const float c_globalOutlineWidth = 2.0f;

// If canvas' zoom multiplier less or equal than this constant
// the render will be simplified
const float c_changeRenderZoomMultiplier = 0.4f;



// --------- CANVAS ---------
// CANVAS GENERAL CONSTANTS

const short c_zoomMaxBound = 0;
const short c_zoomMinBound = -10;

// CANVAS RENDER CONSTANTS

const float c_diffCoeffForPinConnectionCurves = 0.4f;
const short c_xDiffFunctionBlendPoint = 100;
const short c_maxYDiff = 50;
const short c_maxDiffsSum = 150;



// --------- PINS ----------
// PINS RENDER CONSTANTS

const float c_pinFontSizeCoef = 0.65f;

// D stands for diameter
const float c_normalPinD = 17.0f;
const float c_pinConnectLineWidth = 2.5f;

// PINS GENERAL CONSTANTS

const char c_pinDataSeparator = '/';
const QString c_mimeFormatForPinConnection = "PinData";



// -------- NODES ---------
// NODES RENDER CONSTANTS

const short c_nodeNameSize = 15;
const float c_nodeRoundingRadius = 20.0f;
const float c_nodeNameRoundedRectSizeY = 0.2f;
const float c_nodeNameRoundedRectSizeX = 0.5f;
const float c_nodeMaxOutlineWidth = 3.0f;



// ------- COLORS ---------
const QColor c_paletteDefaultColor{ QColor(0x28, 0x2A, 0x3A) };
const QColor c_dotsColor{ QColor(0xE7, 0xF6, 0xF2) };
const QColor c_nodesBackgroundColor{ QColor(0x39, 0x5B, 0x64, 200) };
const QColor c_nodesOutlineColor{ QColor(0xA5, 0xC9, 0xCA, 200) };

}
