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
    // Create the shader source factory for loading shaders from strings/memory
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    // Define vertex shader source (HLSL)
    const char* VSSource = R"(
        struct PSInput
        {
            float3 pos : SV_POSITION;
            float3 color : SV_COLOR;
        };

        void main(in int VertId : SV_VertexID, out PSInput PSIn)
        {
            float4 Pos[3];
            Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
            Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
            Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

            float3 Col[3];
            Col[0] = float3(1.0, 0.0, 0.0); // red
            Col[1] = float3(0.0, 1.0, 0.0); // green
            Col[2] = float3(0.0, 0.0, 1.0); // blue

            PSIn.Pos   = Pos[VertId];
            PSIn.Color = Col[VertId];
        }
    )";

    // Define pixel shader source (HLSL)
    const char* PSSource = R"(
        struct PSInput
        {
            float3 pos : SV_POSITION;
            float3 color : SV_COLOR;
        };

        struct PSOutput
        {
            float4 color : SV_TARGET;
        };

        void main(in PSInput PSIn, out PSOutput PSOut) : SV_TARGET
        {
            PSOut.Color = float4(PSIn.Color.rgb, 1.0);
        }
    )";

    // Create shaders
    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.UseCompinedTextureSamplers = true;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    // Create vertex shader
    RefCntAutoPtr<IShader> pVS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = "Triangle vertex shader";
    ShaderCI.Source = VSSource;
    m_pDevice->CreateShader(ShaderCI, &pVS);

    // Create pixel shader
    RefCntAutoPtr<IShader> pPS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = "Triangle pixel shader";
    ShaderCI.Source = PSSource;
    m_pDevice->CreateShader(ShaderCI, &pPS);

    // Create a Pipeline State
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    
    // Set render target format
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;

    // Set primitive topology 
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Disable culling for now
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;

    // Disable depth testing for now
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    // Attach shaders to the pipeline state
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPipelineState);

    // Create a shader resource binding (even though we don't use any resources in this example)
    m_pPipelineState->CreateShaderResourceBinding(&m_pSRB, true);
}

void ForgedFlightApp::Render()
{
    // Clear the back buffer
    const float ClearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set the pipeline state
    m_pImmediateContext->SetPipelineState(m_pPipelineState);

    // Commit shader resources (none in this example)
    m_pImmediateContext->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Draw the triangle (3 vertices, no vertex buffer - using SV_VertexID)
    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;
    m_pImmediateContext->Draw(drawAttrs);
}

void ForgedFlightApp::Update(double CurrTime, double ElapsedTime)
{
    // Update application state. We don't have anything to update in this example.
}