#include "Partial.h"
#include <cmath>

double XX(double x, double y)
{
    if (sin(x) > 0.0)
    {
        return -0.1;
    }
    return -1.0;
}

double TT(double x, double y)
{
    return 1.0;
}

double InitialValue(double x, double y)
{
    return 0.5 * exp(-100.0 * (x - 0.5) * (x - 0.5) - 100.0 * (y + 0.5) * (y + 0.5));
}

int main()
{
    ///////////////////// Set problem parameters ////////////////////////////
    Partial::RectangleParams params; // domain is rectangle
    params.nodesX = 150; // mesh resolution
    params.nodesY = 150;
    params.x0 = -1.0f; params.x1 = 1.0f; // domain extent
    params.y0 = -1.0f; params.y1 = 1.0f;
    params.t1 = 20.0f; // time extent
    params.XX = XX; // equation coefficients
    params.YY = XX;
    params.TT = TT;
    params.InitialValue = InitialValue; // intial condition
    params.deltaTime = 0.001; // timestep
    params.renderFreq = 10; // number of updates between renders
    Partial::SolveRectangle(params, "testdemosolution.psln"); // solve the problem, save to file

    ////////////////////// Render the solution ///////////////////////////
    Partial::Renderer::Initialize();
    Partial::Renderer renderer;
    renderer.saveDirectory = "DemoSolutionImages"; // directory where images of solutions will be saved
    renderer.LoadSolution("testdemosolution.psln");

    return 0;
}