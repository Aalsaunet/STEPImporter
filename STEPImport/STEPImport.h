#include "stdafx.h"

#define STEP_API extern "C" __declspec(dllexport)

struct IndexedVertex {
	Vector3 vertex;
	int32_t index;
	IndexedVertex(Vector3 v, int32_t i) : vertex(v), index(i) {};

	bool operator<(const IndexedVertex& otherFace) const {
		if (this->vertex < otherFace.vertex)
			return true;
		return false;
	}
};

STEP_API int ImportSTEPFile(const char* filepath, int32_t* numberOfSubShapes);
void FindAllSubShapes(int32_t* numberOfSubShapes);
STEP_API int ProcessSubShape(int32_t subshapeIndex, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
void RetrieveFaces(TopoDS_Shape &shape, std::vector<IndexedVertex>& indexedVerticies);
void RemoveDuplicateVerticesAndUpdateIndices(std::vector<IndexedVertex>& indexedVerticies, std::vector<Vector3>& vertices, std::vector<int32_t>& indices);
void MarshalGeometryData(std::vector<Vector3>& vertices, std::vector<int32_t>& indices, char** vertexBuffer, int32_t* vertexEntryCount, char** indexbuffer, int32_t* indexCount);
STEP_API int ClearStepModelData();
void TestStepImporter();
int main();
