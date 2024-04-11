#pragma once
#include "CollisionDetection.h"
#include "Debug.h"

namespace NCL {
    using namespace NCL::Maths;
    namespace CSC8503 {
        template<class T>
        class OcTree;

        template<class T>
        struct OcTreeEntry
        {
            Vector3 pos;
            Vector3 size;
            T object;

            OcTreeEntry(T obj, Vector3 pos, Vector3 size)
            {
                object = obj;
                this->pos = pos;
                this->size = size;
            };
        };

        template<class T>
        class OcTreeNode
        {
        public:
            typedef std::function<void(std::list<OcTreeEntry<T>>&)> OcTreeFunc;
        protected:
            friend class OcTree<T>;

            OcTreeNode()
            {
                children = nullptr;
                hasChild = false;
                this->position = Vector3();
                this->size = Vector3();
            };

            OcTreeNode(Vector3 pos, Vector3 size)
            {
                children = nullptr;
                hasChild = false;
                this->position = pos;
                this->size = size;
            };

            ~OcTreeNode()
            {
                delete[] children;
            };

            void Insert(T& object, const Vector3& objectPos, const Vector3& objectSize, int depthLeft, int maxSize)
            {
                if (!CollisionDetection::AABBTest(objectPos, position, objectSize, size))
                {
                    return;
                }
                if (hasChild)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        children[i].Insert(object, objectPos, objectSize, depthLeft - 1, maxSize);
                    }
                }
                else
                {
                    contents.push_back(OcTreeEntry<T>(object, objectPos, objectSize));
                    if ((int)contents.size() > maxSize && depthLeft > 0)
                    {
                        Split();
                        for (auto& entry : contents)
                        {
                            for (int j = 0; j < 8; j++)
                            {
                                children[j].Insert(entry.object, entry.pos, entry.size, depthLeft - 1, maxSize);
                            }
                        }
                        contents.clear();
                    }
                }
            };

            void Split()
            {
                if (!children)
                {
                    Vector3 halfSize = size / 2.0f;
                    children = new OcTreeNode<T>[8];
                    for (int i = 0; i < 8; i++)
                    {
                        Vector3 OffsetDir;
                        OffsetDir.x = i % 2 == 0 ? -1 : 1;
                        OffsetDir.z = (i / 2) % 2 == 0 ? 1 : -1;
                        OffsetDir.y = i / 4 == 0 ? -1 : 1;
                        children[i] = OcTreeNode<T>(position + halfSize * OffsetDir, halfSize);
                    }
                }
                hasChild = true;
            };

            void Reset()
            {
                if (hasChild)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        children[i].Reset();
                    }
                    hasChild = false;
                }
                else
                {
                    contents.clear();
                }
            };


            void OperateOnContents(OcTreeFunc& func)
            {
                if (hasChild)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        children[i].OperateOnContents(func);
                    }
                }
                else
                {
                    if (!contents.empty())
                    {
                        func(contents);
                    }
                }
            };

            bool RayOriginInside(const Vector3& rayOriginLocal)
            {
                bool inside = rayOriginLocal.x >= -size.x || rayOriginLocal.x <= size.x;
                inside &= rayOriginLocal.y >= -size.y || rayOriginLocal.y <= size.y;
                inside &= rayOriginLocal.z >= -size.z || rayOriginLocal.y <= size.z;
                return inside;
            }

            bool RayIntersect(Ray& ray, OcTreeFunc& func)
            {
                RayCollision collision;
                if (!CollisionDetection::RayBoxIntersection(ray, position, size, collision))
                {
                    if (!RayOriginInside(ray.GetPosition() - position))
                        return false;
                }


                if (hasChild)
                {
                    bool anyIntersect = false;
                    for (int i = 0; i < 8; i++)
                    {
                        anyIntersect |= children[i].RayIntersect(ray, func);
                    }
                    return anyIntersect;
                }
                else
                {
                    func(contents);
                }
                return true;
            }

            void Print(int depth)
            {
                printf("Pos: (%f,%f,%f), Size: (%f,%f,%f)\n", position.x, position.y, position.z, size.x, size.y, size.z);
                if (children)
                {
                    printf("{\n");
                    for (int i = 0; i < 8; i++)
                    {
                        children[i].Print(depth + 1);
                    }
                    printf("}\n");
                }
            }

        protected:
            Vector3 position;
            Vector3 size;
            std::list<OcTreeEntry<T>> contents;
            OcTreeNode<T>* children;
            bool hasChild;
        };
    }
}


namespace NCL {
    using namespace NCL::Maths;
    namespace CSC8503 {
        template<class T>
        class OcTree
        {
        public:
            OcTree(Vector3 size, int inMaxDepth = 6, int inMaxSize = 5)
                : maxDepth(inMaxDepth)
                , maxSize(inMaxSize)
            {
                root = OcTreeNode<T>(Vector3(), size);
            };
            ~OcTree() {};

            void Insert(T object, const Vector3& pos, const Vector3& size)
            {
                root.Insert(object, pos, size, maxDepth, maxSize);
            };

            void Reset()
            {
                root.Reset();
            };

            void OperateOnContents(typename OcTreeNode<T>::OcTreeFunc func) {
                root.OperateOnContents(func);
            };

            bool RayIntersect(Ray& ray, typename OcTreeNode<T>::OcTreeFunc func)
            {
                return root.RayIntersect(ray, func);
            };

            void PrintTree()
            {
                root.Print(0);
            }

        protected:
            OcTreeNode<T> root;
            int maxDepth;
            int maxSize;
        };
    }
}