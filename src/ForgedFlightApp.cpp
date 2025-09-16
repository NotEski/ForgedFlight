#include "ForgedFlightApp.h"
#include "ShaderSourceStreamFactory.h"
#include "MapHelper.hpp"

void ForgedFlightApp::Initialize(const NativeAppInitAttrib& InitAttrib)
{
    // Call base class initialization
    NativeApp::Initialize(InitAttrib);

    // Create the pipeline state object
    CreatePipelineState();

    // Create the vertex buffer
    CreateVertexBuffer();
}

void ForgedFlightApp::CreatePipelineState()
{
    
}