#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

namespace Partial
{
    inline double ZeroFunc1D(double) { return 0.0; }
    inline double ZeroFunc2D(double, double) { return 0.0; };

    ////////////////////////////////////////////////////////
    /////// Rectangle //////////////////////////////////////
    ////////////////////////////////////////////////////////

    // Parameters for a general 2nd order linear PDE problem in an axis-aligned rectangular domain
    // This struct handles both time-dependent and independent problems. In a time-independent problem, the time-related parameters are simply ignored
    struct RectangleParams
    {
    public:
        // Coefficients of equation
        // XXu_xx + XYu_xy + YYu_yy + Xu_x + Yu_y + TTu_tt + Tu_t = A
        double (*XX)(double, double) = ZeroFunc2D;
        double (*XY)(double, double) = ZeroFunc2D;
        double (*YY)(double, double) = ZeroFunc2D;
        double (*X)(double, double) = ZeroFunc2D;
        double (*Y)(double, double) = ZeroFunc2D;
        double (*TT)(double, double) = ZeroFunc2D;
        double (*T)(double, double) = ZeroFunc2D;
        double (*A)(double, double) = ZeroFunc2D;

        // Extent of rectangle
        double x0 = 0.0, x1 = 1.0, y0 = 0.0, y1 = 1.0;

        // Number of nodes in x/y directions
        uint32_t nodesX = 25;
        uint32_t nodesY = 25;

        // Time extent
        double t0 = 0.0, t1 = 1.0;

        // Time step
        double deltaTime = 0.01;
        // Number of frames between renders
        uint32_t renderFreq = 1;

        // Boundary conditions
        double (*BCLeft)(double) = ZeroFunc1D;
        double (*BCRight)(double) = ZeroFunc1D;
        double (*BCTop)(double) = ZeroFunc1D;
        double (*BCBottom)(double) = ZeroFunc1D;

        // Initial condition
        double (*InitialValue)(double, double) = ZeroFunc2D;
        double (*InitialVelocity)(double, double) = ZeroFunc2D;
    };

    void SolveRectangle(const RectangleParams& params, const std::string& outputPath);
    void SolveRectangleTimeIndependent(const RectangleParams& params, const std::string& outputPath);
    void SolveRectangleTimeDependent(const RectangleParams& params, const std::string& outputPath);

    ////////////////////////////////////////////////////////
    /////// Curvilinear Rectangle //////////////////////////
    ////////////////////////////////////////////////////////

    struct CurvilinearRectangleParams
    {
        // WIP
    };

    void SolveCurvilinearRectangle(const CurvilinearRectangleParams& params, const std::string& outputPath);
    void SolveCurvilinearRectangleTimeIndependent(const CurvilinearRectangleParams& params, const std::string& outputPath);
    void SolveCurvilinearRectangleTimeDependent(const CurvilinearRectangleParams& params, const std::string& outputPath);

    ////////////////////////////////////////////////////////
    /////// Unstructured Mesh //////////////////////////////
    ////////////////////////////////////////////////////////

    struct UnstructuredMeshParams
    {
        // WIP
    };

    void SolveUnstructuredMesh(const UnstructuredMeshParams& params, const std::string& outputPath);
    void SolveUnstructuredMeshTimeIndependent(const UnstructuredMeshParams& params, const std::string& outputPath);
    void SolveUnstructuredMeshTimeDependent(const UnstructuredMeshParams& params, const std::string& outputPath);

    //////////////// Direct solvers for linear systems ////////////////
    std::vector<double> SolveTridiagonal(std::vector<double> diagonal, std::vector<double> below, std::vector<double> above, std::vector<double> vec); // don't pass diagonals by reference since we modify copies within the function

    // Adds data to end of file
    void FileAddUInt(std::ofstream& file, uint32_t value);
    void FileAddInt(std::ofstream& file, int32_t value);
    void FileAddDouble(std::ofstream& file, double value);

    // Loads data at given offset in binary file
    void FileLoadUInt(std::ifstream& file, uint64_t offset, uint32_t count, uint32_t* buffer);
    void FileLoadInt(std::ifstream& file, uint64_t offset, uint32_t count, int32_t* buffer);
    void FileLoadDouble(std::ifstream& file, uint64_t offset, uint32_t count, double* buffer);

    ////////////////////////////////////////////////////////
    /////// Solution Renderer //////////////////////////////
    ////////////////////////////////////////////////////////

    class Renderer
    {
    public:
        static void Initialize();
        Renderer();
        void Update();
        int32_t LoadSolution(const std::string& filepath);
        void JumpToFrame(uint32_t frame);
        double GetNode(uint32_t i, uint32_t j);
        int32_t RecompileShaders();

        inline GLFWwindow* GetWindow() { return window;  }
    private:
        void JumpToFrameRectangle(uint32_t frame);

    private:
        static GLFWwindow* window;

        enum Mode {
            Rectangle = 0,
            CurvilinearMesh,
            UnstructuredMesh,
        };
        Mode mode; // current mode

        // params of current solution being rendered
        RectangleParams rectangle;
        CurvilinearRectangleParams curvilinearRectangle;
        UnstructuredMeshParams unstructuredMesh;

        std::vector<double> nodes; // values at nodes

        int32_t currentFrame = 0;
        std::ifstream solutionFile;

        float cameraR, cameraTheta, cameraPhi;
        glm::mat4 cameraView, cameraProjection;
        void UpdateCameraViewMatrix();

        // vertex array/buffer
        uint32_t graphVA, graphVB;
        
        // shader/uniforms
        Shader shader;
        uint32_t ModelLoc, ViewLoc, ProjectionLoc;

        // colormap texture
        uint32_t colorMap;
    };
}