#include "world.hpp"

namespace our {

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children

    /// @brief this function will deserialize a json array of entities and add the new entities with their children to the current world
    /// @param data: the json array of entities
    /// @param parent: the parent of the entities
    void World::deserialize(const nlohmann::json& data, Entity* parent){
        if(!data.is_array()) return;
        /// loop around the data
        for(const auto& entityData : data){ 
            //TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData".
            Entity* entity = add(); /// add the entity to the current world
            if(parent != nullptr) /// I think this is not necessary
                entity->parent = parent;
            entity->deserialize(entityData); /// deserialize the entity
            /// check if the entity has children to deserialize
            if(entityData.contains("children")){
                //TODO: (Req 8) Recursively call this world's "deserialize" using the children data
                // and the current entity as the parent
                /// deserialize the children of the entity
                deserialize(entityData["children"], entity);
                
            }
        }
    }

}