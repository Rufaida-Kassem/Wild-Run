#include "entity.hpp"
#include "../deserialize-utils.hpp"
#include "../components/component-deserializer.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function returns the transformation matrix from the entity's local space to the world space
    // Remember that you can get the transformation matrix from this entity to its parent from "localTransform"
    // To get the local to world matrix, you need to combine this entities matrix with its parent's matrix and
    // its parent's parent's matrix and so on till you reach the root.

    /// @brief  This function returns the transformation matrix from the entity's local space to the world space
    /// @return The transformation matrix from the entity's local space to the world space
    /// @note this function can be recursive
    glm::mat4 Entity::getLocalToWorldMatrix() const {
        //TODO: (Req 8) Write this function
        Entity *p = parent;  /// get the parent of the entity
        glm::mat4 localToWorld = localTransform.toMat4(); /// get the local to world matrix of the entity --> the direct world of the entity is its parent
        /// loop around the parents and multiply the local to world matrix of the entity with the local to world matrix of the parent
        while (p)
        {
            /// get the local to world matrix of the parent and multiply it with the local to world matrix of the entity
            /// the order of the multiplicatino is like this: parent * entity cause we want to go from the entity to the parent
            localToWorld = p->localTransform.toMat4() * localToWorld; // take care --> the order matters
            p = p->parent;
        }
        //return glm::mat4(1.0f); // remove this line
        return localToWorld;
    }

    // Deserializes the entity data and components from a json object
    void Entity::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        name = data.value("name", name);
        localTransform.deserialize(data);
        if(data.contains("components")){
            if(const auto& components = data["components"]; components.is_array()){
                for(auto& component: components){
                    deserializeComponent(component, this);
                }
            }
        }
    }

}