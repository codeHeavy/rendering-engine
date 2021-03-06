#pragma once

#include <d3d11.h>
#include "Camera.h"
#include "Lights.h"
#include <unordered_map>
#include "Entity.h"
#include "Water.h"
#include "Resources.h"
#include "Terrain.h"

class Renderer
{
	ID3D11DepthStencilView *depthStencilView;
	ID3D11RenderTargetView *backBufferRTV;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swapChain;
	Camera *camera;
	std::unordered_map<std::string, Light*> lights;
	Resources * resources;

	//shadow data
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
	ID3D11SamplerState* shadowSampler;
	ID3D11ShaderResourceView* shadowSRV;

public:
	void SetShadowViewProj(DirectX::XMFLOAT4X4, DirectX::XMFLOAT4X4, ID3D11SamplerState*, ID3D11ShaderResourceView*);
	void SetDepthStencilView(ID3D11DepthStencilView *depthStencilView);
	void SetResources(Resources* rsrc);
	void ClearScreen(const float color[4]);
	void SetCamera(Camera* cam);
	void SetLights(std::unordered_map<std::string, Light*> lightsMap);
	void Draw(Entity *entity);
	void Draw(Terrain *entity);
	void DrawAsLineList(Entity *entity);
	void Present();
	Renderer(ID3D11DeviceContext *ctx, ID3D11RenderTargetView *backBuffer, ID3D11DepthStencilView *depthStencil, IDXGISwapChain *inSwapChain);
	void SetBackBuffer(ID3D11RenderTargetView* backBufferRTV);
	~Renderer();
};