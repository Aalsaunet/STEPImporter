#include "stdafx.h"

#define STEP_API extern "C" __declspec(dllexport)

STEP_API int ImportSTEPFile(const char* filepath, int32_t* numberOfSubShapes);
void FindAllSubShapes(int32_t* numberOfSubShapes);
STEP_API int ProcessSubShape(int32_t* subshapeIndex, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
void RetrieveFaces(TopoDS_Shape &shape);
void RemoveDuplicateVerticesAndUpdateIndices();
void MarshalGeometryData(char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
void ClearGeometryData();
int testGet3DGeometry();
int main();
