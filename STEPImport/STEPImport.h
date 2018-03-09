#include "stdafx.h"

#define STEP_API extern "C" __declspec(dllexport)

STEP_API int Get3DGeometry(const char* filepath, char** geometricalbuffer, char** indexbuffer, int32_t* vertexElements, 
						   int32_t* normalElements, int32_t* uv2Elements, int32_t* triangleElements);

void MarshalGeometryData(int32_t * vertexElements, int32_t * normalElements, int32_t * uv2Elements, char ** geometricalbuffer, int32_t * triangleElements, char ** indexbuffer);
void GenerateFaces(TopoDS_Shape &shape);
void calculateNormals();
void RemoveDuplicatesAndWriteToBuffer();
void ClearGeometryData();
int testGet3DGeometry();
int main();
