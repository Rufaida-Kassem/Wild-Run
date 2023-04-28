#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our
{

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR 1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL 3

    class Mesh
    {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        //.--------------------------------------------------------------------
        //. VBO (vertex buffer object): store verticies data                  |
        //. VAO (vecter array object): store the specifications of the data stored in buffer  |
        //. EBO (element buffer object): store the indicies of the verticies  |
        //.--------------------------------------------------------------------
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;

    public:
        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)
        {
            // TODO: (Req 2) Write this function
            //  remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            elementCount = (GLsizei)elements.size();
            //. generation of vertex array
            //. @param n=1 --> generates only one vertex array
            //. @param *array = &VAO --> that stores all of the state (attributes) needed to supply vertex data
            glGenVertexArrays(1, &VAO);
            //. bind the vertex array. function that takes the generated vertex array as a parameter
            glBindVertexArray(VAO);
            //. generation of vertex buffer object
            //. @param n=1 --> generates only one vertex buffer object
            //. @param *array = &VBO --> where the verticies will be stored
            glGenBuffers(1, &VBO);
            //. bind the vertex buffer object function that takes the generated vertex buffer object as a parameter
            //. and target = GL_ARRAY_BUFFER to bind vertex array
            //. note that it is the same function using for binding texture data buffer just change the target
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            //. creates and initializes a buffer object's data store
            //. @param target buffer = GL_ARRAY_BUFFER --> specify the target buffer
            //. @parram size = vertices.size() * sizeof(Vertex) --> the size of vertex buffer object
            //. @param *data = (void *)vertices.data() --> which is the array to be saved into the buffer
            //. @param usage = GL_STATIC_DRAW --> STATIC : The data store contents will be stored once.
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void *)vertices.data(), GL_STATIC_DRAW);
            //. Enable a vertex location array
            //. @param attribute = location --> used the defined constanst above
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            //. configure an array of vertex location data (Vertex Array will store these commands as configuration)
            //. @param index = ATTRIB_LOC_POSITION(0)
            //. size = 3 --> Specifies the number of components per vertex location.
            //. type = GL_FLOAT --> Specifies type of eeach data
            //. normalized = false (we do not devide by the maximum to normalize)
            //. stride = sizeof(Vertex) --> Specifies the byte offset between consecutive vertex locations.
            //. offest = (void *)offsetof(Vertex, position) --> specify the offset of the component for data stord to be stored
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, position));
            //. Enable a vertex color array
            //. @param attribute = color --> used the defined constanst above
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            //. define an array of vertex color data (Vertex Array will store these commands as configuration)
            //. @param index = ATTRIB_LOC_COLOR(1)
            //. size = 4 --> Specifies the number of components per vertex color (R,G,B,alpha).
            //. normalized = true (devision by 255 to be ranged from 0 to 1)
            //. stride = sizeof(Vertex) --> Specifies the byte offset between consecutive generic vertex colors.
            //. offest = (void *)offsetof(Vertex, color) --> specify the offset of the component for data stord to be stored
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void *)(offsetof(Vertex, color)));
            //. Enable a vertex texcoord array
            //. @param attribute = texcoord --> used the defined constanst above
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            //. define an array of vertex texcoord data (Vertex Array will store these commands as configuration)
            //. @param index = ATTRIB_LOC_TEXCOORD(2)
            //. size = 2 --> Specifies the number of components per vertex texcoord.
            //. normalized = false
            //. stride = sizeof(Vertex) --> Specifies the byte offset between consecutive vertex texcoords.
            //. (void *)offsetof(Vertex, tex_coord) --> specify the offset of the component for data stord to be stored
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(offsetof(Vertex, tex_coord)));
            //. Enable a vertex normal array
            //. @param attribute = ATTRIB_LOC_NORMAL --> used the defined constanst above
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            //. define an array of vertex normal data (Vertex Array will store these commands as configuration)
            //. @param index = ATTRIB_LOC_NORMAL(4)
            //. size = 3 --> Specifies the number of components per vertex normal.
            //. normalized = false
            //. stride = sizeof(Vertex) --> Specifies the byte offset between consecutive vertex texcoords.
            //. (void *)offsetof(Vertex, tex_coord) --> specify the offset of the component for data stord to be stored
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, false, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));
            //. generation of element buffer object
            //. @param n=1 --> generates only one element buffer object
            //. @param *array = &EBO --> that stores elements
            glGenBuffers(1, &EBO);
            //. bind the element buffer object. function that takes the generated  element buffer object as a parameter
            //. and target = GL_ELEMENT_ARRAY_BUFFER to bind Vertex array indices
            //. not that it is the same function using for binding texture data buffer just change the target
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            //. creates and initializes a buffer object's data store
            //. @param target buffer = GL_ELEMENT_ARRAY_BUFFER --> for the purpose of Vertex array indices
            //. @parram size = elementCount * sizeof(GL_UNSIGNED_INT) --> the size of the buffer
            //. @param *data = (void *)elements.data() --> which is the array to be saved into the buffer
            //. @param usage = GL_STATIC_DRAW --> STATIC : The data store contents will be modified once and used many times.
            //.                                   DRAW : The data store contents are modified by the application
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(GL_UNSIGNED_INT), (void *)elements.data(), GL_STATIC_DRAW);
        }

        // this function should render the mesh
        void draw()
        {
            // TODO: (Req 2) Write this function
            //. bind the vertex array object. function that takes the generated vertex array object as a parameter
            //. and target = GL_ARRAY_BUFFER to bind vertex array
            //. not that it is the same function using for binding texture data buffer just change the target
            glBindVertexArray(VAO);
            //. rendereing from array
            //. @param mode = GL_TRIANGLES
            //. @param count = elementCount --> number of elements to be rendered
            //. @param type = GL_UNSIGNED_INT --> Specifies the type of the values in indices
            //. @param index = 0 --> Specifies a pointer to the location where the indices are stored.
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void *)0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh()
        {
            // TODO: (Req 2) Write this function
            // delete vertex array object, vertex buffer object and element buffer object
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}
