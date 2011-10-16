
// 頂点シェーダ
// Debug:   fxc /E vs_main /T vs_5_0 /Fh src/metasequoia.vs.h /Od /Vn g_vs_metasequoia /Zi ../000/shaders/004_metasequoia/metasequoia.vs
// Release: fxc /E vs_main /T vs_5_0 /Fh src/metasequoia.vs.h /O1 /Vn g_vs_metasequoia ../000/shaders/004_metasequoia/metasequoia.vs

struct VS_IN
{
	float3 position : IN_POSITION;
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
	VSout.color    = float4( 0.8f, 0.8f, 0.8f, 1.0f );
//	VSout.color    = float4( 0.0f, 1.0f, 0.0f, 1.0f );   // ワイヤーフレーム用

	return VSout;
}

