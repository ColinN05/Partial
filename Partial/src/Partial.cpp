#include "Partial.h"
#include <iostream>
#include <iterator>
#include "stb_image.h"

// Color codes for in-console solution rendering
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace Partial
{
    void SolveRectangle(const RectangleParams& params, const std::string& outputPath)
    {
        if (params.T == ZeroFunc2D && params.TT == ZeroFunc2D)
        {
            SolveRectangleTimeIndependent(params, outputPath);
        }
        else
        {
            SolveRectangleTimeDependent(params, outputPath);
        }
    }

    void SolveRectangleTimeIndependent(const RectangleParams& params, const std::string& outputPath)
    {
        std::cout << "Solving time independent problem." << std::endl;

        std::vector<double> nodes(params.nodesX * params.nodesY, 0.0);
        
        double tolerance = 0.0001;

        double deltaX = (params.x1 - params.x0) / (params.nodesX - 1.0);
        double deltaY = (params.y1 - params.y0) / (params.nodesY - 1.0);

        std::vector<double> diagonal(params.nodesX, 0.0);
        std::vector<double> below(params.nodesX - 1, 0.0);
        std::vector<double> above(params.nodesX - 1, 0.0);
        std::vector<double> vec(params.nodesX, 0.0);
        std::vector<double> solution(params.nodesX, 0.0);

        for (uint32_t it = 0; it < 10000; it++)
        {
            for (uint32_t j = 1; j < params.nodesY-1; j++)
            {
                diagonal[0] = 1.0; vec[0] = 0.0;
                diagonal[params.nodesX - 1] = 1.0; 
                vec[params.nodesX - 1] = 1.0;
                for (uint32_t i = 1; i < params.nodesX-1; i++)
                {
                    double x,y;
                    x = params.x0 + i * deltaX;
                    y = params.y0 + j * deltaY;

                    double A,B,C,D,E,F;
                    A = (*params.XX)(x,y);
                    B = (*params.XY)(x,y); 
                    C = (*params.YY)(x,y);
                    D = (*params.X)(x,y);
                    E = (*params.Y)(x,y);
                    F = (*params.A)(x,y);

                    diagonal[i] = -2.0 * A/(deltaX * deltaX) - 2.0 * C/(deltaY * deltaY) + B/(deltaX * deltaY) - D/deltaX - E/deltaY;
                    above[i] = A/(deltaX * deltaX) - B/(deltaX * deltaY) + D/deltaX;
                    below[i-1] = A/(deltaX * deltaX);
                    vec[i] = F + nodes[i + (j + 1) * params.nodesX] * (-C/(deltaY * deltaY) + B/(deltaX * deltaY) - E/deltaY)+nodes[i + (j - 1) * params.nodesX] * (-C/(deltaY * deltaY));
                }
                solution = SolveTridiagonal(diagonal, below, above, vec);
                for (uint32_t i = 0; i < params.nodesX; i++)
                {
                    nodes[i + j * params.nodesX] = solution[i];
                }
            }
            for (uint32_t i = 0; i < params.nodesX; i++)
            {
                nodes[i + 0 * params.nodesX] = 0.0;
                nodes[i + (params.nodesY-1) * params.nodesX] = 0.0;
            }
        }

        /*
        for (int j = 0; j < params.nodesX; j++)
        {
            std::string line = "";
            for (int i = 0; i < params.nodesY; i++)
            {
                line += std::to_string(nodes[i + j * params.nodesX]);
                line += " ";
            }
            std::cout << line << std::endl;
        }
        */

        for (uint32_t j = 0; j < params.nodesX; j++)
            {
            std::string line = "";
            for (uint32_t i = 0; i < params.nodesY; i++)
            {
                double value = nodes[i + j * params.nodesX];
                if (value < 0.15)
                {
                    line += std::string(ANSI_COLOR_BLUE "@@" ANSI_COLOR_RESET);
                }
                else if (value < 0.25)
                {
                    line += std::string(ANSI_COLOR_CYAN "@@" ANSI_COLOR_RESET);
                }
                else if (value < 0.45)
                {
                    line += std::string(ANSI_COLOR_GREEN "@@" ANSI_COLOR_RESET);
                }
                else if (value < 0.6)
                {
                    line += std::string(ANSI_COLOR_YELLOW "@@" ANSI_COLOR_RESET);
                }
                else
                {
                    line += std::string(ANSI_COLOR_RED "@@" ANSI_COLOR_RESET);
                }
                }
                std::cout << line << std::endl;

            }
    }

    void SolveRectangleTimeDependent(const RectangleParams& params, const std::string& outputPath)
    {
        std::cout << "Solving time dependent problem." << std::endl;

        std::ofstream outputFile(outputPath, std::ios::binary);

        // Add problem parameters to beginning of solution file
        FileAddUInt(outputFile, uint32_t(0)); // 0 = rectangle
        FileAddDouble(outputFile, params.x0); FileAddDouble(outputFile, params.x1);
        FileAddDouble(outputFile, params.y0); FileAddDouble(outputFile, params.y1);
        FileAddUInt(outputFile, params.nodesX); FileAddUInt(outputFile, params.nodesY);
        FileAddDouble(outputFile, params.t0); FileAddDouble(outputFile, params.t1);
        FileAddDouble(outputFile, params.deltaTime);
        FileAddUInt(outputFile, params.renderFreq);

        std::vector<std::vector<double>> nodes(params.nodesX, std::vector<double>(params.nodesY, 0.0)); // current value of nodes
        std::vector<std::vector<double>> nodesOld1(params.nodesX, std::vector<double>(params.nodesY, 0.0)); // value of nodes at t - deltaT
        std::vector<std::vector<double>> nodesOld2(params.nodesX, std::vector<double>(params.nodesY, 0.0)); // value of nodes at t - 2deltaT

        double x,y, time=params.t0, deltaX, deltaY;
        deltaX = (double)(params.x1 - params.x0) / ((double)params.nodesX - 1.0);
        deltaY = (double)(params.y1 - params.y0) / ((double)params.nodesY - 1.0);

        uint32_t iteration = 0;
        
        // set initial conditions
        for (uint32_t i = 0; i < params.nodesX; i++)
        {
            for (uint32_t j = 0; j < params.nodesY; j++)
            {
                x = params.x0 + i * deltaX;
                y = params.y0 + j * deltaY;

                // boundary nodes
                if (i == 0)
                {
                    nodesOld1[i][j] = params.BCLeft(y);
                    nodesOld2[i][j] = params.BCLeft(y);
                    continue;
                }
                else if (i == params.nodesX - 1)
                {
                    nodesOld1[i][j] = params.BCRight(y);
                    nodesOld2[i][j] = params.BCRight(y);
                    continue;
                }
                else if (j == 0)
                {
                    nodesOld1[i][j] = params.BCBottom(x);
                    nodesOld2[i][j] = params.BCBottom(x);
                    continue;
                }
                else if (j == params.nodesY - 1)
                {
                    nodesOld1[i][j] = params.BCTop(x);
                    nodesOld2[i][j] = params.BCTop(x);
                    continue;
                }

                // non boundary nodes
                nodesOld2[i][j] = params.InitialValue(x,y);
                nodesOld1[i][j] = nodesOld2[i][j] + params.InitialVelocity(x,y) * params.deltaTime; 
            }
        }

        double alpha = 1.0/deltaX, beta = 1.0/deltaY, gamma = 1.0/params.deltaTime;

        while (time < params.t1)
        {
            // update nodes
            for (uint32_t i = 0; i < params.nodesX; i++)
            {
                for (uint32_t j = 0; j < params.nodesY; j++)
                {
                    x = params.x0 + i * deltaX;
                    y = params.y0 + j * deltaY;

                    // boundary nodes
                    if (i == 0)
                    {
                        nodes[i][j] = params.BCLeft(y);
                        continue;
                    }
                    else if (i == params.nodesX - 1)
                    {
                        nodes[i][j] = params.BCRight(y);
                        continue;
                    }
                    else if (j == 0)
                    {
                        nodes[i][j] = params.BCBottom(x);
                        continue;
                    }
                    else if (j == params.nodesY - 1)
                    {
                        nodes[i][j] = params.BCTop(x);
                        continue;
                    }

                    // non boundary nodes
                    double XX, XY, YY, X, Y, TT, T;
                    XX = params.XX(x,y);
                    XY = params.XY(x,y); 
                    YY = params.YY(x,y);
                    X = params.X(x,y);
                    Y = params.Y(x,y);
                    TT = params.TT(x,y);
                    T = params.T(x,y);

                    nodes[i][j] = -1.0/(TT * gamma * gamma + T * gamma) *
                    (
                        nodesOld1[i+1][j] * (XX * alpha * alpha + X * alpha) +
                        nodesOld1[i][j] * (-2.0 * XX * alpha * alpha - 2.0 * YY * beta * beta - X * alpha - Y * beta - 2.0 * TT * gamma * gamma - T * gamma) +
                        nodesOld1[i-1][j]  * XX * alpha * alpha +
                        nodesOld1[i-1][j-1] * XY * 0.25 * alpha * beta +
                        nodesOld1[i+1][j+1] * XY * 0.25 * alpha * beta + 
                        nodesOld1[i+1][j-1] * (-XY) * 0.25 * alpha * beta +
                        nodesOld1[i-1][j+1] * (-XY) * 0.25 * alpha * beta +
                        nodesOld1[i][j-1] * YY * beta * beta +
                        nodesOld1[i][j+1] * (YY * beta * beta + Y * beta) +
                        nodesOld2[i][j] * TT * gamma * gamma
                    );
                }
            }

            // dump solution data to file
            if (iteration % params.renderFreq == 0)
            {
                for (uint32_t i = 0; i < params.nodesX; i++)
                {
                    for (uint32_t j = 0; j  < params.nodesY; j++)
                    {
                        FileAddDouble(outputFile,nodes[i][j]);
                    }
                }
           }

            nodesOld2 = nodesOld1;
            nodesOld1 = nodes;

            time += params.deltaTime;
            iteration++;
        }
    }

    std::vector<double> SolveTridiagonal(std::vector<double> diagonal, std::vector<double> below, std::vector<double> above, std::vector<double> vec)
    {
        uint32_t size = diagonal.size();

        // forward elimination
        double coeff;
        for (uint32_t pivot_row = 0; pivot_row < size - 1; pivot_row++)
        {
            coeff = below[pivot_row] / diagonal[pivot_row];
            diagonal[pivot_row + 1] -= coeff * above[pivot_row];
            vec[pivot_row + 1] -= coeff * vec[pivot_row];
        }

        // backward substitution
        std::vector<double> solution(size, 0.0);
        solution[size - 1] = vec[size - 1] / diagonal[size - 1];
        for (int32_t i = size - 2; i > -1; i--)
        {
            solution[i] = (vec[i] - above[i] * solution[i+1]) / diagonal[i];
        }
        return solution;
    }

    void FileAddUInt(std::ofstream& file, uint32_t value)
    {
        for (uint32_t p = 0; p < 4; p++)
        {
            file << *((char*)&value+p);
        }
    }
    void FileAddInt(std::ofstream& file, int32_t value)
    {
        for (uint32_t p = 0; p < 4; p++)
        {
            file << *((char*)&value+p);
        }
    }
    void FileAddDouble(std::ofstream& file, double value)
    {
        for (uint32_t p = 0; p < 8; p++)
        {
            file << *((char*)&value+p);
        }
    }

    void FileLoadUInt(std::ifstream& file, uint64_t offset, uint32_t count, uint32_t* buffer)
    {
        file.seekg(offset, std::ios::beg);
        file.read((char*)buffer, count * sizeof(uint32_t));
    }
    void FileLoadInt(std::ifstream& file, uint64_t offset, uint32_t count, int32_t* buffer)
    {
        file.seekg(offset, std::ios::beg);
        file.read((char*)buffer, count * sizeof(int32_t));
    }   
    void FileLoadDouble(std::ifstream& file, uint64_t offset, uint32_t count, double* buffer)
    {
        file.seekg(offset, std::ios::beg);
        file.read((char*)buffer, count * sizeof(double));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    GLFWwindow* Renderer::window;

    void Renderer::Initialize()
    {
        if (!glfwInit())
            printf("Error: Failed to initialize GLFW.\n");

        window = glfwCreateWindow(900,900, "Solution", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            printf("Error: Failed to create window.\n");
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!status)
        {
            std::cout << "Failed to initialize glad!" << '\n';
        }
    }

    Renderer::Renderer()
        : cameraR(1.0f), cameraTheta(0.0f), cameraPhi(1.0f),
        cameraProjection(glm::perspective(glm::radians(90.0f), 1.0f,0.1f,100.0f)),
        shader("Partial/src/vertexshader.glsl", "Partial/src/fragmentshader.glsl")
    {
        UpdateCameraViewMatrix();

        glGenVertexArrays(1, &graphVA);
        glBindVertexArray(graphVA);

        glGenBuffers(1, &graphVB);
        glBindBuffer(GL_ARRAY_BUFFER, graphVB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(0 * sizeof(float))); // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float))); // Value
        glEnableVertexAttribArray(2);

        shader.Use();

        glEnable(GL_DEPTH_TEST);

        // Get MVP uniform locations
        ModelLoc = glGetUniformLocation(shader.GetID(), "Model");
        ViewLoc = glGetUniformLocation(shader.GetID(), "View");
        ProjectionLoc = glGetUniformLocation(shader.GetID(), "Projection");

        glClearColor(0.1f,0.1f,0.1f,1.0f);

        // Load texture
        int width, height, nrChannels;
        unsigned char* data = stbi_load("Partial/src/temperature_colormap.jpg", &width, &height, &nrChannels, 0);

        // Create OpenGL texture and load the data into it
        glGenTextures(1, &colorMap);
        glBindTexture(GL_TEXTURE_2D, colorMap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    int32_t Renderer::LoadSolution(const std::string& filepath)
    {
        solutionFile = std::ifstream(filepath, std::ios::in | std::ios::binary);

        uint32_t solutionType;
        FileLoadUInt(solutionFile, 0, 1, &solutionType);

        mode = (Mode)solutionType;

        uint32_t numFrames = 0;

        switch(solutionType)
        {
        case Mode::Rectangle: // rectangle
            // load parameters for rectangle struct
            FileLoadDouble(solutionFile, 4, 1, &rectangle.x0);
            FileLoadDouble(solutionFile, 4+8, 1, &rectangle.x1);
            FileLoadDouble(solutionFile, 4+8+8, 1, &rectangle.y0);
            FileLoadDouble(solutionFile, 4+8+8+8, 1, &rectangle.y1);
            FileLoadUInt(solutionFile,   4+8+8+8+8, 1, &rectangle.nodesX);
            FileLoadUInt(solutionFile,   4+8+8+8+8+4, 1, &rectangle.nodesY);
            FileLoadDouble(solutionFile, 4+8+8+8+8+4+4, 1, &rectangle.t0);
            FileLoadDouble(solutionFile, 4+8+8+8+8+4+4+8, 1, &rectangle.t1);
            FileLoadDouble(solutionFile, 4+8+8+8+8+4+4+8+8, 1, &rectangle.deltaTime);
            FileLoadUInt(solutionFile,   4+8+8+8+8+4+4+8+8+8, 1, &rectangle.renderFreq);
            nodes.reserve(rectangle.nodesX * rectangle.nodesY);

            numFrames = (rectangle.t1 - rectangle.t0) / (rectangle.deltaTime * rectangle.renderFreq);
            break;
        case Mode::CurvilinearMesh:
            break;
        case Mode::UnstructuredMesh:
            break; 
        }

        // Render loop
        while (!glfwWindowShouldClose(window))
        {
            JumpToFrame(currentFrame);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                std::cout << currentFrame << std::endl;
                currentFrame++;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                currentFrame--;
            }
            if (currentFrame < 0) currentFrame = 0;
            if (currentFrame > numFrames - 1) currentFrame = numFrames - 1;
            Update();
        }

        return 0;
    }

    void Renderer::JumpToFrame(uint32_t frame)
    {
        switch (mode)
        {
        case Mode::Rectangle:
            JumpToFrameRectangle(frame);
            break;
        case Mode::CurvilinearMesh:
            break;
        case Mode::UnstructuredMesh:
            break;
        }
    }

    void Renderer::JumpToFrameRectangle(uint32_t frame)
    {
        currentFrame = frame;

        // position of data in solution file
        uint64_t offset = 4 + 8 + 8 + 8 + 8 + 4 + 4 + 8 + 8 + 8 + 4 + (uint64_t)rectangle.nodesX * (uint64_t)rectangle.nodesY * sizeof(double) * (uint64_t)frame;

        FileLoadDouble(solutionFile, offset, rectangle.nodesX * rectangle.nodesY, &nodes[0]);
        nodes.resize(rectangle.nodesX * rectangle.nodesY);


        // vertex data for solution graph
        std::vector<float> graphVertices(rectangle.nodesX * rectangle.nodesY * 2 * 3 * 7, 0.0f);

        // fill graphVertices
        for (int i = 0; i < rectangle.nodesX-1; i++)
        {
            for (int j = 0; j < rectangle.nodesY; j++)
            {
                float x, y;
                float deltaX, deltaY;
                deltaX = (rectangle.x1 - rectangle.x0) / (rectangle.nodesX - 1.0f);
                deltaY = (rectangle.y1 - rectangle.y0) / (rectangle.nodesY - 1.0f);
                x = rectangle.x0 + i * deltaX;
                y = rectangle.y0 + j * deltaY;

                //// Triangle 1
                glm::vec3 normal = glm::cross(
                    glm::vec3(deltaX, GetNode(i + 1, j) - GetNode(i, j), 0.0f),
                    glm::vec3(deltaX, GetNode(i + 1, j + 1) - GetNode(i, j), deltaY)
                );
                graphVertices.insert(graphVertices.end(),
                    {
                        x,(float)GetNode(i,j),y,normal.x,normal.y,normal.z, (float)GetNode(i,j), // vertex 1
                        x + deltaX, (float)GetNode(i + 1,j), y, normal.x, normal.y, normal.z, (float)GetNode(i + 1,j), // 2
                        x + deltaX, (float)GetNode(i + 1,j + 1), y + deltaY, normal.x, normal.y, normal.z, (float)GetNode(i + 1,j + 1) // 3
                    });


                //// Triangle 2
                normal = glm::cross(
                    glm::vec3(deltaX, GetNode(i + 1, j + 1) - GetNode(i, j), deltaY),
                    glm::vec3(0.0f, GetNode(i, j + 1) - GetNode(i, j), deltaY)
                );
                graphVertices.insert(graphVertices.end(),
                    {
                        x, (float)GetNode(i,j), y, normal.x, normal.y, normal.z, (float)GetNode(i,j), // vertex 1
                        x + deltaX, (float)GetNode(i + 1,j + 1), y + deltaY, normal.x, normal.y, normal.z, (float)GetNode(i + 1,j + 1), // 2
                        x, (float)GetNode(i,j + 1), y + deltaY, normal.x, normal.y, normal.z, (float)GetNode(i,j + 1) // 3
                    });

            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, graphVB);
        glBufferData(GL_ARRAY_BUFFER, graphVertices.size() * sizeof(float), &graphVertices[0], GL_STATIC_DRAW);
    }

    void Renderer::Update()
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(cameraView));
        glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, glm::value_ptr(cameraProjection));

        glm::mat4 model(1.0f);
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(graphVA);
        glBindTexture(GL_TEXTURE_2D, colorMap);
        glDrawArrays(GL_TRIANGLES, 0, rectangle.nodesX * rectangle.nodesY * 2 * 3 * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        //glBlendFunc(GL_SCALE_BY_ONE_HALF_NV, GL_ONE_MINUS_SRC_ALPHA);

        if (glfwGetKey(window, GLFW_KEY_S))
        {
            cameraR *= 1.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_W))
        {
            cameraR /= 1.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            cameraPhi -= 0.005;
            if (cameraPhi < 0.0f)
            {
                cameraPhi = 0.001f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            cameraPhi += 0.005;
            if (cameraPhi > 3.1415f)
            {
                cameraPhi = 3.1415f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            cameraTheta -= 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            cameraTheta += 0.001;
        }

        UpdateCameraViewMatrix();
    }

    double Renderer::GetNode(uint32_t i, uint32_t j)
    {
        return nodes[i * rectangle.nodesY + j];
    }

    void Renderer::UpdateCameraViewMatrix()
    {
        glm::vec3 cameraPos(
            cameraR * sinf(cameraPhi) * cosf(cameraTheta),
            cameraR * cosf(cameraPhi),
            cameraR * sinf(cameraPhi) * sinf(cameraTheta)
        );
        cameraView = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0,1.0,0.0));
    }

    int32_t Renderer::RecompileShaders()
    {
        shader = Shader("Partial/src/vertexshader.glsl", "Partial/src/fragmentshader.glsl");
        return 0;
    }
}