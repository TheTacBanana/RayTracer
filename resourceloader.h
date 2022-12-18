#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <assert.h>

typedef unsigned int ResourceIndex;

int resourceCounter = 0;
template <class T>
int GetResourceID(){
    static int resourceID = resourceCounter++;
    return resourceID;
}

struct ResourcePool{
    const unsigned int MAX_RESOURCES = 128;
    char* pData {nullptr};
    std::size_t elementSize{ 0 };
    unsigned int nextID = 0;

    std::unordered_map<std::string, ResourceIndex> map; 
    std::vector<ResourceIndex> freeIDS;

    ResourcePool(std::size_t elementSize){
        this->elementSize = elementSize;
        pData = new char[elementSize * MAX_RESOURCES];
    }

    ~ResourcePool(){
        delete[] pData;
    }

    inline void* Get(ResourceIndex id){
        return pData + id * elementSize;
    }

    ResourceIndex GetID(std::string key){
        if (map.count(key)){
            return map[key];
        } else if (freeIDS.empty()){
            ResourceIndex id = nextID;
            nextID++;
            map[key] = id;
            return id;
        } else {
            ResourceIndex id = freeIDS.back();
            freeIDS.pop_back();
            return id;
        }
    }

    void Remove(std::string key){
        ResourceIndex id = map[key];
        map.erase(key);
        freeIDS.push_back(id);
        delete Get(id);
    }
};

class ResourceLoader{
    public:
        static inline std::vector<ResourcePool*> resourcePools;

        static inline std::string resourcePath;

        static void SetResourcePath(std::string resourceFolderPath){
            resourcePath = resourceFolderPath;
        }

        template<typename T>
        static void LoadResource(std::string stringIdentifier, std::string path1){
            int resourceID = GetResourceID<T>();

            if (resourcePools.size() <= resourceID){
            resourcePools.resize(resourceID + 1, nullptr);
            }
            if (resourcePools[resourceID] == nullptr){
                resourcePools[resourceID] = new ResourcePool(sizeof(T));
            }

            ResourceIndex id = resourcePools[resourceID]->GetID(stringIdentifier);
            new (resourcePools[resourceID]->Get(id)) T(resourcePath + path1);
        }

        template<typename T>
        static void LoadResource(std::string stringIdentifier, std::string path1, std::string path2){
            int resourceID = GetResourceID<T>();

            if (resourcePools.size() <= resourceID){
            resourcePools.resize(resourceID + 1, nullptr);
            }
            if (resourcePools[resourceID] == nullptr){
                resourcePools[resourceID] = new ResourcePool(sizeof(T));
            }

            ResourceIndex id = resourcePools[resourceID]->GetID(stringIdentifier);
            new (resourcePools[resourceID]->Get(id)) T(resourcePath + path1, resourcePath + path2);
        }

        template<typename T>
        static T* GetResource(std::string stringIdentifier){
            int resourceID = GetResourceID<T>();

            // Check within range
            assert(resourceID < resourcePools.size());

            ResourceIndex id = resourcePools[resourceID]->GetID(stringIdentifier);
            return static_cast<T*>(resourcePools[resourceID]->Get(id));
        }   

        template<typename T>
        static ResourcePool* GetResourcePool(){
            int resourceID = GetResourceID<T>();

            if (resourcePools.size() <= resourceID){
            resourcePools.resize(resourceID + 1, nullptr);
            }
            if (resourcePools[resourceID] == nullptr){
                resourcePools[resourceID] = new ResourcePool(sizeof(T));
            }

            return resourcePools[resourceID];
        }      
};

#endif