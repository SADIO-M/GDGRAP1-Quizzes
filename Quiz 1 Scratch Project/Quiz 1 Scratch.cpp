/*
    Assignment 02 - Linear Transformations

    Created by: SADIO (GDGRAP1-S22)
    Latest revision: January 30, 2025, 5:20 PM

    Submitted on: January 30, 2025
*/

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

float x = 0.0f, y = 0.0f, z = 0.0f;
float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
float theta_x = 0.0f, theta_y = 0.0f;

mat4 identity_matrix = mat4(1.0f);
mat4 transformation_matrix;

void key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {

    //TRANSLATION with WASD. Makes it translate up, down, left, and right
    if (key == GLFW_KEY_D && action == GLFW_REPEAT) x += 0.1f;
    if (key == GLFW_KEY_A && action == GLFW_REPEAT) x -= 0.1f;
    if (key == GLFW_KEY_W && action == GLFW_REPEAT) y += 0.1f;
    if (key == GLFW_KEY_S && action == GLFW_REPEAT) y -= 0.1f;

    //SCALING with Q and E. E to increase and Q to decrease.
    if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
        scale_x += 0.5f;
        scale_y += 0.5f;
        scale_z += 0.5f;
    }
    if (key == GLFW_KEY_Q && action == GLFW_REPEAT) {
        scale_x -= 0.5f;
        scale_y -= 0.5f;
        scale_z -= 0.5f;
    }

    //ROTATION with arrow keys
    if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)  theta_y -= 5.0f;     
    if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) theta_y += 5.0f;
    if (key == GLFW_KEY_UP && action == GLFW_REPEAT)    theta_x -= 5.0f;
    if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)  theta_x += 5.0f;

}

int main(void)
{
    GLFWwindow* window;
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(640, 480, "[SADIO] Assignment 2 - Linear Transformations", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetKeyCallback(window, key_Callback);

    /* Load and create a file */
    fstream vertSrc("Shaders/Shader.vert");
    stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    /* Convert stream into a character array */
    string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    fstream fragSrc("Shaders/Shader.frag");
    stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    /* Create a fragment shader
    *  Assign source to fragment shader
    *  Compile the Fragment Shader
    */
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    /*
    * Create the shader program
    * Attach the compiled vertex & fragment shader
    */
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    /*Initialize the FF variables*/
    string path = "3D/bunny.obj";
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> material;
    string warning, error;
    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes, //Overall def
        &shapes,  //Refers to the object itself
        &material, //Refers to the texture/image
        &warning,
        &error,
        path.c_str()
    );
    /* We need to instruct the EBO from the Mesh Data */
    vector<GLuint> meshIndices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        meshIndices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    /* Render loop until the user closes the window */
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * attributes.vertices.size(), &attributes.vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE, 3 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * meshIndices.size(),
        meshIndices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(shaderProg);
    glBindVertexArray(VAO);

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int transformLocation = glGetUniformLocation(shaderProg, "transform");

        //Translation
        transformation_matrix = translate(identity_matrix, vec3(x, y, z));
        
        //Scaling
        transformation_matrix = scale(transformation_matrix, vec3(scale_x, scale_y, scale_z));
        
        //Rotation
        if(theta_x) transformation_matrix = rotate(transformation_matrix, radians(theta_x), normalize(vec3(1, 0, 0)));
        if(theta_y) transformation_matrix = rotate(transformation_matrix, radians(theta_y), normalize(vec3(0, 1, 0)));
        
        glUniformMatrix4fv(transformLocation, 1, GL_FALSE, value_ptr(transformation_matrix));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
