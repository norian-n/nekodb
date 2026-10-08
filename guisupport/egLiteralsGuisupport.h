#pragma once

#include <cstring>
#include <unordered_set>

const std::string detailsLayerIDName        ("egDetailsLayerID"); // egDataNode.cpp

const std::string layersSetsFilePostfix     ("_egLayersSet"); // layers sets file name postfix
const std::string oneLayerFilePostfix       ("_egOneLayer");  // one layer file name postfix
const std::string oneLayerDarkNodesPostfix  ("_egLayerDarkNodesLink");  // one layer file name postfix

const std::string layersMetainfoStorageName ("egLayersMetainfo"); // set and blueprint
const std::string layersFieldNameName       ("layersName");
const std::string layersFieldTopLayerIDName ("layersTopLayerID");

const std::string layersSetBPName           ("egLayersSetBP");
const std::string layerNodesSets_FieldName  ("nodesNames");
const std::string layerLinks_FieldName      ("linksNames");
const std::string layerDarkNodesLink_FieldName  ("darkNodesFreeLinkName");
const std::string layerWidth_FieldName       ("layerWidth");
const std::string layerHeight_FieldName      ("layerHeight");
const std::string layerParentID_FieldName    ("parentLayerID");

const std::string nodeWidgetBPName          ("egNodeWidgetsSetBP");
const std::string nodeWidgetFieldLabelName  ("widgetLabel");
const std::string nodeWidgetFieldXName      ("cornerX");
const std::string nodeWidgetFieldYName      ("cornerY");
const std::string nodeWidgetFieldWName      ("width");
const std::string nodeWidgetFieldHName      ("height");
const std::string nodeWidgetFieldColorName  ("fillColor");
const std::string nodeWidgetFieldImageName  ("image");
const std::string nodeWidgetDetailsIDName   ("detailsLayerID");
const std::string nodeWidgetFieldNodesName  ("boundNodesSet");
const std::string nodeWidgetFieldNodeIDName ("boundNodeID");
const std::string nodeWidgetFieldDescName   ("widgetDescription");

const std::string linkWidgetBPName          ("egLinkWidgetsSetBP");
const std::string linkWidgetStartX_FName    ("startPointX");
const std::string linkWidgetStartY_FName    ("startPointY");
const std::string linkWidgetEndX_FName      ("endPointX");
const std::string linkWidgetEndY_FName      ("endPointY");
const std::string linkWidgetPortFrom_FName  ("portFrom");
const std::string linkWidgetPortTo_FName    ("portTo");
const std::string linkWidgetCoordFrom_FName ("sideCoordFrom");
const std::string linkWidgetCoordTo_FName   ("sideCoordTo");
const std::string linkWidgetLineType_FName  ("lineType");

const std::string palettesMetainfoName      ("egPalettesMetainfo");
const std::string palettesNameField         ("palettesName");

const std::string palettesSetBPName         ("palettesSetBP");
const std::string onePaletteBPName          ("onePaletteBP");

const std::string paletteFieldIndexName     ("paletteIndex");
const std::string paletteFieldNodesName     ("paletteNodesSet");
const std::string paletteFieldNodeIDName    ("paletteNodeID");
const std::string paletteWidgetNameName     ("widgetName");
const std::string paletteWidgetColorName    ("fillColor");


// const std::unordered_set<std::string> egPaletteWidgetFields = {"paletteIndex"};