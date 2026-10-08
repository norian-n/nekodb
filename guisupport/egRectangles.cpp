#include "egRectangles.h"

void alignToGrid(int& coord, int gridCellSize, int canvasIndent)
{
    if (coord < canvasIndent)
        coord = canvasIndent;
    int modulo = coord % gridCellSize;
    if (! modulo)
        return;
    if (modulo < gridCellSize/2)
        coord -= modulo;
    else
        coord += gridCellSize-modulo;
    // cout << "alignToGrid() modulo: " << dec << modulo << " coord: " << coord << endl;
}

void resetToOrigSize  (egSize& size) {
    size.scaledW = size.origW;
    size.scaledH = size.origH;
}
void resetToOrigPoint (egPoint& point) {
    point.scaledX = point.origX;
    point.scaledY = point.origY;
}
void resetToOrigRect  (egRect& rect) {
    resetToOrigPoint(rect.corner);
    resetToOrigSize (rect.size);
}

void origToScaledScalar (int origVal, int& scaledVal, int zoomFactor) {
    scaledVal = origVal * (maxZoom - zoomFactor)/maxZoom;
}
void scaledToOrigScalar (int scaledVal, int& origVal, int zoomFactor) {
    origVal = scaledVal * maxZoom / (maxZoom - zoomFactor);
}

void origToScaledCanvas (int origVal, int& scaledVal, int zoomFactor, int canvas) {
    scaledVal = origVal * (maxZoom - zoomFactor)/maxZoom + canvas;
}
void scaledToOrigCanvas (int scaledVal, int& origVal, int zoomFactor, int canvas) {
    origVal = (scaledVal - canvas) * maxZoom / (maxZoom - zoomFactor);
}

void origToScaledSize (egSize& size, int zoomFactor) {
    origToScaledScalar(size.origW, size.scaledW, zoomFactor);
    origToScaledScalar(size.origH, size.scaledH, zoomFactor);    
}
void scaledToOrigSize(egSize& size, int zoomFactor) {
    scaledToOrigScalar(size.scaledW, size.origW, zoomFactor);
    scaledToOrigScalar(size.scaledH, size.origH, zoomFactor);
}

void origToScaledPointCanvas (egPoint& point, int zoomFactor, egPoint& canvas) {
    origToScaledCanvas(point.origX, point.scaledX, zoomFactor, canvas.scaledX);
    origToScaledCanvas(point.origY, point.scaledY, zoomFactor, canvas.scaledY);    
}
void scaledToOrigPointCanvas(egPoint& point, int zoomFactor, egPoint& canvas) {
    scaledToOrigCanvas(point.scaledX, point.origX, zoomFactor, canvas.scaledX);
    scaledToOrigCanvas(point.scaledY, point.origY, zoomFactor, canvas.scaledY);
}

void origToScaledPoint (egPoint& point, int zoomFactor) {
    origToScaledScalar(point.origX, point.scaledX, zoomFactor);
    origToScaledScalar(point.origY, point.scaledY, zoomFactor);    
}
void scaledToOrigPoint(egPoint& point, int zoomFactor) {
    scaledToOrigScalar(point.scaledX, point.origX, zoomFactor);
    scaledToOrigScalar(point.scaledY, point.origY, zoomFactor);
}

void origToScaledRect   (egRect& rect, int zoomFactor) {
    origToScaledPoint(rect.corner, zoomFactor);
    origToScaledSize (rect.size, zoomFactor);
}
void scaledToOrigRect   (egRect& rect, int zoomFactor) {
    scaledToOrigPoint(rect.corner, zoomFactor);
    scaledToOrigSize (rect.size, zoomFactor);
}

void origToScaledRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas) {
    origToScaledPointCanvas(rect.corner, zoomFactor, canvas);
    origToScaledSize (rect.size, zoomFactor);    
}
void scaledToOrigRectCanvas   (egRect& rect, int zoomFactor, egPoint& canvas) {
    scaledToOrigPointCanvas(rect.corner, zoomFactor, canvas);
    scaledToOrigSize (rect.size, zoomFactor);    
}

void scaledToOrigLayer   (egRect& rect, int zoomFactor) {
    rect.corner.origX = 0;
    rect.corner.origY = 0;
    rect.corner.scaledX = 0;
    rect.corner.scaledY = 0;
    scaledToOrigSize (rect.size, zoomFactor);
}
void origToScaledLayer   (egRect& rect, int zoomFactor) {
    origToScaledSize (rect.size, zoomFactor);
    rect.corner.scaledX = (rect.size.origW - rect.size.scaledW) / 2;
    rect.corner.scaledY = (rect.size.origH - rect.size.scaledH) / 2;
}