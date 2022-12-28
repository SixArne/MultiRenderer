// Matrix push constantsd
float4x4 gWorldViewProj: WorldViewProjection;
float4x4 gViewInverse: VIEWINVERSE;
float4x4 gWorld: WORLD;

// Textures
Texture2D gDiffuseMap: DiffuseMap;

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;
	
	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;
	
	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;
	
	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;
	
	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};

// Light directions
float3 gLightDir: LightDirection;

// Constants
float PI = float(3.14159);
float INTENSITY = float(7.0);
float SHININESS = float(25.0);
float3 AMBIENT = float3(0.025, 0.025, 0.025);

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror, Clamp, Border
	AddressV = Wrap;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;// or Mirror, Clamp, Border
	AddressV = Wrap;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;// or Mirror, Clamp, Border
	AddressV = Wrap;
};

struct VS_INPUT
{
	float3 Position: POSITION;
	float2 Uv: TEXCOORD;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
};

struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: WORLDPOSITION;
	float2 Uv: TEXCOORD0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
};




/// Vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Uv = input.Uv;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorld);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorld);
	output.WorldPosition = mul(input.Position, gWorld);

	return output;
}

float CalculatePhong(float3 specular, float phongExponent, float3 lightDirection, float3 viewDirection, float3 normal)
{
	float3 reflected = reflect(normal, lightDirection);
	float angle = max(0, dot(reflected, viewDirection));
	float phongValue = specular * pow(angle, phongExponent);

	return phongValue;
}

float3 CalculateLambert(float kd, float3 color)
{
	return mul(color, kd / PI);
}

// Pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{

	return gDiffuseMap.Sample(samPoint, input.Uv);
	//float3 diffuse = CalculateLambert(1.f, diffuseMapSample);
	//float3 finalColor = ((diffuse * INTENSITY) + AMBIENT);

}

// Technique
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}