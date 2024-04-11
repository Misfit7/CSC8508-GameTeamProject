#pragma once
#include "NavigationMap.h"
#include <string>
namespace NCL {
	namespace CSC8503 {
		class RailObject;
		struct GridNode {
			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			Vector3		position;

			float f;
			float g;

			int type;

			RailObject* rail;

			GridNode() {
				for (int i = 0; i < 4; ++i) {
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
			}
			~GridNode() {	}

			void SetType(int t) {
				type = t;
			}

			void SetRail(RailObject* r) {
				rail = r;
			}
			RailObject* GetRail() const {
				return rail;
			}
		};

		class NavigationGrid : public NavigationMap {
		public:
			NavigationGrid();
			NavigationGrid(const std::string& filename);
			~NavigationGrid();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;
			bool FindPath2(const Vector3& from, const Vector3& to, NavigationPath& outPath);
			bool FindPath(const Vector3& from, const Vector3& to, float& outF);
			Vector3 FindNearestTree(const Vector3& position);
			Vector3 FindNearestRock(const Vector3& position);

			GridNode& GetGridNode(int index) const {
				return allNodes[index];
			}
			void SetGridNode(GridNode* g) {
				allNodes = g;
			}

			int GetGridWidth() const {
				return gridWidth;
			}

			int GetGridHeight() const {
				return gridHeight;
			}

		protected:
			bool		NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			GridNode* RemoveBestNode(std::vector<GridNode*>& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;

			GridNode* allNodes;
		};
	}
}

