#include "NativeApp.h"
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"

using namespace Diligent;

class ForgedFlightApp : public NativeApp
{
public:
    virtual void Initialize(const NativeAppInitAttrib& InitAttrib) override final;
    virtual void Render() override final;
    virtual void Update(double CurrTime, double ElapsedTime) override final;
    virtual const Char* GetAppTitle() const override final { return "Forged Flight"; }

private:
    void CreatePipelineState();
    void CreateVertexBuffer();

    RefCntAutoPtr<IPipelineState>           m_pPSO;
    RefCntAutoPtr<IBuffer>                  m_pVertexBuffer;
    RefCntAutoPtr<IShaderResourceBinding>   m_pSRB;
}