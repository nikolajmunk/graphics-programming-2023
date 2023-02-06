#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/core/Data.h>
#include <iostream>
#include <numbers>

int buildShaderProgram();
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//float time{}, angle{};
float angle{};
float length = (float)std::sqrt(2) / 2;
const float TWOPI = std::numbers::pi * 2;
const float ninety = std::numbers::pi / 2;
float offsetX, offsetY;
float indentAmount = 0.5f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    DeviceGL deviceGL;

    // glfw window creation
    // --------------------
    Window window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");
    if (!window.IsValid())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    deviceGL.SetCurrentWindow(window);
    if (!deviceGL.IsReady())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    int shaderProgram = buildShaderProgram();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const int edgeVertexCount = 18;

    //float vertices[] = {
    //    -0.5f, -0.5f, 0.0f, // bottom left  
    //     0.5f, -0.5f, 0.0f, // bottom right 
    //     0.5f,  0.5f, 0.0f,  // top right
    //    -0.5f,  0.5f, 0.0f // top left
    //};

    //float vertices[] = {
    //     0.0f,  0.0f, 0.0f,
    //    -0.5f, -0.5f, 0.0f, // bottom left  
    //     0.5f, -0.5f, 0.0f, // bottom right 
    //     0.5f,  0.5f, 0.0f,  // top right
    //    -0.5f,  0.5f, 0.0f // top left
    //};

    float vertices[(edgeVertexCount+1)*3]{};

    float slice = TWOPI / edgeVertexCount;

    for (size_t i = 1; i < edgeVertexCount+1; i++)
    {
        float newLength = length * (i % 2 == 0 ? indentAmount : 1.0f);

        float x = std::sin(slice * (i-1)) * newLength;
        float y = std::cos(slice * (i-1)) * newLength;

        vertices[i * 3] = x;
        vertices[i * 3 + 1] = y;
    }

    /*for (size_t i = 1; i < edgeVertexCount; i ++)
    {
        angle = slice * i;

        float x = std::sin(angle) * length;
        float y = std::cos(angle) * length;

        vertices[i*3] = x;
        vertices[i*3+1] = y;
    }*/

    /*unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };*/



    unsigned int indices[edgeVertexCount*3]{};

    for (size_t i = 0; i < edgeVertexCount; i++)
    {
        int x = i * 3;
        int y = i * 3 + 1;
        int z = i * 3 + 2;
        indices[i*3] = 0;
        int q = (i + 1) % (edgeVertexCount) + 1;
        int r = (i) % (edgeVertexCount) + 1;
        indices[i*3 + 1] = q;
        indices[i*3 + 2] = r;
    }

    VertexBufferObject vbo;
    VertexArrayObject vao;
    ElementBufferObject ebo;

    // unsigned int VBO, VAO;
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    vao.Bind();

    vbo.Bind();
    int vertexCount(sizeof(vertices) / sizeof(float)); // Divide byte size by the size of a single float to get number of elements.
    std::span verticesSpan(vertices, vertexCount);
    vbo.AllocateData(verticesSpan);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ebo.Bind();
    int indicesCount = sizeof(indices) / sizeof(unsigned int);
    std::span indicesSpan(indices, indicesCount);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSpan.size_bytes(), indicesSpan.data(), GL_STATIC_DRAW);
    ebo.AllocateData(indicesSpan);

    VertexAttribute position(Data::GetType<float>(), 3);

    vao.SetAttribute(0, position, 0, 0);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    vbo.Unbind();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    vao.Unbind();

    ebo.Unbind();

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!window.ShouldClose())
    {
        // input
        // -----
        processInput(window.GetInternalWindow());

        // render
        // ------
        //time += 0.1f;
        //angle = time * 0.01f;
        deviceGL.Clear(Color(0.2f, 0.3f, 0.3f, 1.0f));

        // draw our first triangle
        glUseProgram(shaderProgram);

        float offset{};
        /*for (size_t i = 0; i < vertexCount / 3; i++)
        {
            float radians = ninety * i;
            int vertX = 3 * i;
            int vertY = vertX + 1;
            float x = std::sin(angle + radians) * length;
            float y = std::cos(angle + radians) * length;
            vertices[vertX] = x;
            vertices[vertY] = y;
        }*/

        vertices[0] += offsetX;
        vertices[1] += offsetY;

        for (size_t i = 1; i < edgeVertexCount + 1; i++)
        {
            float newLength = length * (i % 2 == 0 ? indentAmount : 1.0f);

            float x = std::sin(slice * (i - 1) + angle) * newLength;
            float y = std::cos(slice * (i - 1) + angle) * newLength;

            vertices[i * 3] = vertices[0] + x;
            vertices[i * 3 + 1] = vertices[1] + y;
        }

        //for (size_t i = 0; i < edgeVertexCount+1; i++)
        //{
        //    vertices[i * 3] += offsetX;
        //    vertices[i * 3 + 1] += offsetY;
        //}

        vao.Bind(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        vbo.Bind();
        vbo.UpdateData(verticesSpan);
        //glDrawArrays(GL_TRIANGLES, 0, vertexCount); // Uses dynamic vertex count. Should this be static?
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
        // vertexArrayObject.Unbind(); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.SwapBuffers();
        deviceGL.PollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    
    
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // This is now done in the destructor of DeviceGL
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float amount = 0.0006f;

    offsetX = -amount * glfwGetKey(window, GLFW_KEY_LEFT) + amount * glfwGetKey(window, GLFW_KEY_RIGHT);
    offsetY = -amount * glfwGetKey(window, GLFW_KEY_DOWN) + amount * glfwGetKey(window, GLFW_KEY_UP);

    angle += amount * (glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_Q));
    indentAmount += amount * (glfwGetKey(window, GLFW_KEY_PERIOD) - glfwGetKey(window, GLFW_KEY_COMMA));
}

// build the shader program
// ------------------------
int buildShaderProgram() 
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        //"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // Shape color
        "   FragColor = vec4(0.0f, 0.5f, 0.7f, 1.0f);\n" // Shape color
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
