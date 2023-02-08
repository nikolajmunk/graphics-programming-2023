#define STB_PERLIN_IMPLEMENTATION

#include "TerrainApplication.h"
#include <stb_perlin.h>
// (todo) 01.1: Include the libraries you need

#include <cmath>
#include <iostream>
#include <vector>

//bool useDepth;

// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

Color colorFromheight(float height)
{
    if (height < -0.7f)
    {
        return Color(0.06f, 0.08f, 0.16f, 1.0f);
    }
    else if (height < -0.1f)
    {
        return Color(0.18f, 0.24f, 0.48f, 1.0f);
    }
    else if (height < 0.1f)
    {
        return Color(0.7f, 0.65f, 0.48f, 1.0f);
    }
    else if (height < 0.5f)
    {
        return Color(0.47f, 0.58f, 0.41f, 1.0f);
    }
    else
    {
        return Color(0.78f, 0.82f, 0.74f, 1.0f);
    }
}

// (todo) 01.8: Declare an struct with the vertex format
struct Vertex
{
    Vector3 position;
    Vector2 uv;
    Color color;
};


TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(64), m_gridY(64), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();

    // (todo) 01.1: Create containers for the vertex position
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;
    const Color mainColor(0.0f, 0.5f, 1.0f, 1.0f);

    // (todo) 01.1: Fill in vertex data
    float distanceX = 1.0f / m_gridX;
    float distanceY = 1.0f / m_gridY;
    float frequency = 3.0f;
    float amplitude = 0.3f;
    for (GLuint x = 0; x < m_gridX + 1; x++)
    {
        for (GLuint y = 0; y < m_gridY + 1; y++)
        {
            Vertex v;
            float xPos = float(x) * distanceX - 0.5f;
            float yPos = float(y) * distanceY - 0.5f;
            float z = stb_perlin_fbm_noise3(xPos * frequency, yPos * frequency, 0.0, 2, 0.5, 6) * amplitude;
            v.position = Vector3(xPos, yPos, z);

            v.uv = Vector2(x, y);

            float colorfrom{ (z / amplitude)/2 + 0.5f };
            v.color = colorFromheight(z / amplitude);

            vertices.push_back(v);
        }
    }

    for (GLuint j = 0; j < m_gridY; j++)
    {
        for (GLuint i = 0; i < m_gridX; i++)
        {
            int offsetY = j * (m_gridX + 1);
            indices.push_back(offsetY + i);
            indices.push_back(offsetY + i + 1);
            indices.push_back(offsetY + (m_gridX + 1) + i);

            indices.push_back(offsetY + (m_gridX + 1) + i);
            indices.push_back(offsetY + i + 1);
            indices.push_back(offsetY + (m_gridX + 1) + i + 1);

        }
    }

    m_vao.Bind();
    m_vbo.Bind();


    std::span<Vertex> verticesSpan(vertices);
    m_vbo.AllocateData(verticesSpan);
    int offset = 0;

    VertexAttribute position(Data::Type::Float, 3);
    m_vao.SetAttribute(0, position, 0, sizeof(Vertex));
    offset += position.GetSize();

    VertexAttribute uv(Data::Type::Float, 2);
    m_vao.SetAttribute(1, uv, offset, sizeof(Vertex));
    offset += uv.GetSize();

    VertexAttribute color(Data::Type::Float, 4);
    m_vao.SetAttribute(2, color, offset, sizeof(Vertex));

    // (todo) 01.5: Initialize EBO
    m_ebo.Bind();
    m_ebo.AllocateData(std::span(indices));
    // (todo) 01.1: Unbind VAO, and VBO
    m_vao.Unbind();
    m_vbo.Unbind();

    // (todo) 01.5: Unbind EBO
    m_ebo.Unbind();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //useDepth = false;
}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();
    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glUseProgram(m_shaderProgram);

    // (todo) 01.1: Draw the grid
    m_vao.Bind();
    //glDrawArrays(GL_TRIANGLES, 0, m_gridX * m_gridY * 6);
    glDrawElements(GL_TRIANGLES, m_gridX * m_gridY * 6, GL_UNSIGNED_INT, 0);
}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}

void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
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
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }

    //if (GetMainWindow().IsKeyPressed(GLFW_KEY_SPACE) && !GetMainWindow().IsKeyRepeated(GLFW_KEY_SPACE)) {
    //    if (useDepth)
    //    {
    //        glEnable(GL_DEPTH_TEST);
    //        //glDepthFunc(GL_GREATER);
    //    }
    //    else
    //    {
    //        //glDepthFunc(GL_LESS);
    //        glDisable(GL_DEPTH_TEST);
    //    }
    //    useDepth = !useDepth;
    //    std::cout << useDepth;
    //}
}
