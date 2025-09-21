#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "PostProcess/ScreenSpaceAmbientOcclusion/interface/ScreenSpaceAmbientOcclusion.hpp"
#include "PostProcess/Bloom/interface/Bloom.hpp"
#include "PostProcess/Common/interface/PostFXContext.hpp"
#include "Camera.h"

using namespace Diligent;

class AdvancedRenderer
{
public:
    AdvancedRenderer();
    ~AdvancedRenderer() = default;

    // Initialize advanced rendering features
    bool Initialize(IRenderDevice* pDevice, IDeviceContext* pContext, Uint32 width, Uint32 height);
    
    // Rendering pipeline
    void BeginFrame();
    void RenderScene(Camera* camera);
    void ApplyPostProcessing();
    void EndFrame();
    
    // Post-processing toggles
    void SetSSAOEnabled(bool enabled) { m_SSAOEnabled = enabled; }
    void SetBloomEnabled(bool enabled) { m_BloomEnabled = enabled; }
    
    bool IsSSAOEnabled() const { return m_SSAOEnabled; }
    bool IsBloomEnabled() const { return m_BloomEnabled; }

private:
    // Device references
    RefCntAutoPtr<IRenderDevice> m_pDevice;
    RefCntAutoPtr<IDeviceContext> m_pContext;
    
    // Post-processing effects (for future DiligentFX integration)
    // Note: DiligentFX objects will be created when we implement the actual effects
    bool m_pSSAO = false;
    bool m_pBloom = false;
    
    // Render targets for HDR pipeline
    RefCntAutoPtr<ITexture> m_pColorRT;
    RefCntAutoPtr<ITexture> m_pDepthRT;
    RefCntAutoPtr<ITextureView> m_pColorRTV;
    RefCntAutoPtr<ITextureView> m_pDepthDSV;
    RefCntAutoPtr<ITextureView> m_pColorSRV;
    RefCntAutoPtr<ITextureView> m_pDepthSRV;
    
    // Settings
    bool m_SSAOEnabled = true;
    bool m_BloomEnabled = true;
    
    Uint32 m_ScreenWidth = 0;
    Uint32 m_ScreenHeight = 0;
    
    // Helper methods
    void CreateRenderTargets();
    void CreatePostProcessingEffects();
};