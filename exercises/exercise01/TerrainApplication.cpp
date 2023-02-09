#define STB_PERLIN_IMPLEMENTATION

#include "TerrainApplication.h"
#include <stb_perlin.h>
// (todo) 01.1: Include the libraries you need

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

//bool useDepth;
bool wireframeMode{};
bool listenToInput{ true };
float scrollTime{};

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

struct Vertex
{
    Vector3 position;
    Vector2 uv;
    Color color;
    Vector3 normal;
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

void setNormals(std::vector<Vertex>& verts, int xSize, int ySize)
{
    for (int y = 0; y < ySize; y++)
    {
        for (int x = 0; x < xSize; x++)
        {
            int i = y * xSize + x;
            int d = std::max(y - 1, 0) * xSize + x;
            int u = std::min(ySize - 1, y + 1) * xSize + x;
            int r = y * ySize + std::clamp(x + 1, 0, xSize - 1);
            int l = y * ySize + std::clamp(x - 1, 0, xSize - 1);

            Vertex left = verts[l];
            Vertex right = verts[r];
            Vertex up = verts[u];
            Vertex down = verts[d];

            float deltaX((right.position.z - left.position.z) / (right.position.x - left.position.x));
            float deltaY((up.position.z - down.position.z) / (up.position.y - down.position.y));

            Vector3 normal(deltaX, deltaY, 1.0f);
            normal.Normalize();

            verts[i].normal = normal;
            continue;

        }
    }
}

void setHeightColor(std::vector<Vertex>& verts, float time) {
    float frequency = 3.0f;
    float amplitude = 0.3f;
    for (Vertex& v : verts) {
        float z = stb_perlin_fbm_noise3(v.position.x * frequency, v.position.y * frequency, time, 2, 0.5, 6);
        v.position.z = z * amplitude;

        float colorfrom{ z / 2 + 0.5f };
        v.color = colorFromheight(z / amplitude);
    }

}

std::vector<Vertex> vertices;

TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(128), m_gridY(128), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    // Build shaders and store in m_shaderProgram
    BuildShaders();

    // (todo) 01.1: Create containers for the vertex position
    std::vector<GLuint> indices;
    //std::vector<Vertex> vertices;
    const Color mainColor(0.0f, 0.5f, 1.0f, 1.0f);

    // (todo) 01.1: Fill in vertex data
    float distanceX = 1.0f / m_gridX;
    float distanceY = 1.0f / m_gridY;

    int xSize = m_gridX + 1;
    int ySize = m_gridY + 1;

    // Set x,y positions and uvs for each vertex

    for (GLuint y = 0; y < ySize; y++)
    {
        for (GLuint x = 0; x < xSize; x++)
        {
            Vertex v;
            float xPos = float(x) * distanceX - 0.5f;
            float yPos = float(y) * distanceY - 0.5f;
            v.position = Vector3(xPos, yPos, 0.0f);

            v.uv = Vector2(x, y);

            vertices.push_back(v);
        }
    }

    // Set indices

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

    // Set height, color, and normals
    setHeightColor(vertices, scrollTime);
    setNormals(vertices, m_gridX + 1, m_gridY + 1);

    m_vao.Bind();
    m_vbo.Bind();

    std::span<Vertex> verticesSpan(vertices);
    m_vbo.AllocateData(verticesSpan);
    int offset = 0;

    // Set attributes

    VertexAttribute position(Data::Type::Float, 3);
    m_vao.SetAttribute(0, position, 0, sizeof(Vertex));
    offset += position.GetSize();

    VertexAttribute uv(Data::Type::Float, 2);
    m_vao.SetAttribute(1, uv, offset, sizeof(Vertex));
    offset += uv.GetSize();

    VertexAttribute color(Data::Type::Float, 4);
    m_vao.SetAttribute(2, color, offset, sizeof(Vertex));
    offset += color.GetSize();

    VertexAttribute normal(Data::Type::Float, 3);
    m_vao.SetAttribute(3, normal, offset, sizeof(Vertex));

    m_ebo.Bind();
    m_ebo.AllocateData(std::span(indices));

    m_vao.Unbind();
    m_vbo.Unbind();
    m_ebo.Unbind();

    glEnable(GL_DEPTH_TEST);
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

    m_vao.Bind();
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

    // Suuuuuper janky implementation of wireframe toggling using period key

    if (GetMainWindow().IsKeyPressed(GLFW_KEY_PERIOD) && listenToInput)
    {
        listenToInput = false;
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (GetMainWindow().IsKeyReleased(GLFW_KEY_PERIOD) && !listenToInput)
    {
        listenToInput = true;
    }

}
