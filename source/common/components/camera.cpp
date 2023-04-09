#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace our {
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if(cameraTypeStr == "orthographic"){
            cameraType = CameraType::ORTHOGRAPHIC;
        } else {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        //TODO: (Req 8) Complete this function
        //HINT:
        // In the camera space:
        // - eye is the origin (0,0,0)
        // - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        // - up is the direction (0,1,0)
        // but to use glm::lookAt, we need eye, center and up in the world state.
        // Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        // - the eye position which is the point (0,0,0) but after being transformed by M
        // - the center position which is the point (0,0,-1) but after being transformed by M
        // - the up direction which is the vector (0,1,0) but after being transformed by M
        // then you can use glm::lookAt
        
        //determine the position of the camera
        glm::mat4 view = glm::lookAt(
            glm::vec3(M * glm::vec4(0, 0, 0, 1)), //position of the camera  --> rotating
            glm::vec3(M * glm::vec4(0, 0, -1.0, 1)), //position of the object --> let camera look at the center (as the obj exist at origin)
            glm::vec3(M * glm::vec4(0, 1.0, 0, 0) ) //up vector
        );
        return view;
    }

    // Creates and returns the camera projection matrix
    // "viewportSize" is used to compute the aspect ratio
    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const {
        //TODO: (Req 8) Wrtie this function
        // NOTE: The function glm::ortho can be used to create the orthographic projection matrix
        // It takes left, right, bottom, top. Bottom is -orthoHeight/2 and Top is orthoHeight/2.
        // Left and Right are the same but after being multiplied by the aspect ratio
        // For the perspective camera, you can use glm::perspective
        glm::mat4 projection;
        if(cameraType == CameraType::PERSPECTIVE){
            projection = glm::perspective(fovY, float(viewportSize.x) / viewportSize.y, near, far);
        }
        else if(cameraType == CameraType::ORTHOGRAPHIC){
            projection = glm::ortho(-orthoHeight * (float(viewportSize.x) / viewportSize.y) / 2.0f, orthoHeight * (viewportSize.x / viewportSize.y) / 2.0f, -orthoHeight / 2.0f, orthoHeight / 2.0f);
        }
        return projection;
    }
}