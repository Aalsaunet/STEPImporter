#include "stdafx.h"

#define STEP_API extern "C" __declspec(dllexport)

STEP_API int ImportSTEPFile(const char* filepath, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
void IterateShapeHierarchy(std::vector<TopoDS_Shape>& mainSubShapeList, TopoDS_Shape& shape);
void MarshalGeometryData(char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
void RetrieveFaces(TopoDS_Shape &shape);
void RemoveDuplicateVerticesAndUpdateIndices();
void ClearGeometryData();
int testGet3DGeometry();
int main();
