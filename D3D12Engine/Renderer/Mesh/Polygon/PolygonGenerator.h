#pragma once
#include "Polygon.h"

class CPolygonGenerator
{
public:
    std::unique_ptr<Polygons> AssemblePolygon(Polygons* pMainPoly, Polygons* pSubPoly);

    std::unique_ptr<Polygons>           GenFromObjModel(const std::string& name);

private:
    UINT CalcVertexStride(bool bNorm, bool bUV);
    DXGI_FORMAT SetIndexFormat(bool useUint32);
};

