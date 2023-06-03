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

const float c_percentOfCanvasSizeToConsiderNearEdge = 0.14f;

// This is the offset to use with zoomMult=1 to move canvas when the
// cursor is near edge of the canvas during pin drag
const float c_standardPinDragEdgeCanvasMoveValue = 50.0f;

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

const char c_dataSeparator = '/';
const QString c_mimeFormatForPinConnection = "PinData";
const QString c_mimeFormatForNodeFactory = "NewNode";



// -------- NODES ---------
// NODES RENDER CONSTANTS

const short c_nodeNameSize = 15;
const float c_nodeRoundingRadius = 20.0f;
const float c_nodeNameRoundedRectSizeY = 0.2f;
const float c_nodeNameRoundedRectSizeX = 0.5f;
const float c_nodeMaxOutlineWidth = 3.0f;



// ----- NODEFACTORY ---------
// GENERAL

const int c_nfWidgetSpacing = 40;



// ------- COLORS ---------
const QColor c_paletteDefaultColor{ QColor(0x28, 0x2A, 0x3A) };
const QColor c_dotsColor{ QColor(0xE7, 0xF6, 0xF2) };
const QColor c_nodesBackgroundColor{ QColor(0x39, 0x5B, 0x64, 200) };
const QColor c_nfWidgetBackgroundColor{ QColor(0x39, 0x5B, 0x64, 200) };
const QColor c_nodesOutlineColor{ QColor(0xA5, 0xC9, 0xCA, 200) };

}
