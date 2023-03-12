#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
namespace our
{

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const
    {
        // TODO: (Req 3) Write this function
        // we can use glm::translate, glm::yawPitchRoll, and glm::scale to create the needed matrices
        // then we can multiply them together to get the final matrix

        // T - translation matrix: moves the object to the position
        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        // R - rotation matrix: rotates the object by the rotation angles (yaw, pitch, roll)
        glm::mat4 R = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
        // S - scale matrix: scales the object by the scale vector
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        return T * R * S;
    }

    // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json &data)
    {
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale = data.value("scale", scale);
    }

}