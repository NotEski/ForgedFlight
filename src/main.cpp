#include "ForgedFlightApp.h"

int main()
{
    ForgedFlightApp app;

    NativeAppInitAttrib AppInitAttribs;
    AppInitAttribs.WindowTitle  = app.GetAppTitle();
    AppInitAttribs.WindowWidth  = 1280;
    AppInitAttribs.WindowHeight = 720;

    try
    {
        app.Run(AppInitAttribs);
    }
    catch (const std::runtime_error& Err)
    {
        // In case of an error, display it in a message box
        MessageBoxA(nullptr, Err.what(), "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}