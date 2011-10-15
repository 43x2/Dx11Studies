
// 頂点シェーダ
// Debug:   fxc /E vs_main /T vs_5_0 /Fh src/perspective.vs.h /Od /Vn g_vs_perspective /Zi ../000/shaders/003_perspective/perspective.vs
// Release: fxc /E vs_main /T vs_5_0 /Fh src/perspective.vs.h /O1 /Vn g_vs_perspective ../000/shaders/003_perspective/perspective.vs

struct VS_IN
{
	float3 position : IN_POSITION;
	float4 color    : IN_COLOR;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR0;
};


cbuffer VSConstants : register( cb0 )
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}


VS_OUT vs_main( VS_IN VSin )
{
	VS_OUT VSout;

	float4x4 wvp = mul( mul( projection, view ), world );

	VSout.position = mul( float4( VSin.position, 1.0f ), transpose( wvp ) );
	VSout.color    = VSin.color;

	return VSout;
}

