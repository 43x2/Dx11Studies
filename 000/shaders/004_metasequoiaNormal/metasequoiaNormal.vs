
// 頂点シェーダ
// Debug:   fxc /E vs_main /T vs_5_0 /Fh src/metasequoiaNormal.vs.h /Od /Vn g_vs_metasequoiaNormal /Zi ../000/shaders/004_metasequoiaNormal/metasequoiaNormal.vs
// Release: fxc /E vs_main /T vs_5_0 /Fh src/metasequoiaNormal.vs.h /O1 /Vn g_vs_metasequoiaNormal ../000/shaders/004_metasequoiaNormal/metasequoiaNormal.vs

struct VS_IN
{
	float3 position : IN_POSITION;
	float3 normal   : IN_NORMAL;
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

	VSout.position = mul( wvp, float4( VSin.position, 1.0f ) );
	VSout.color    = float4( VSin.normal, 1.0f );

	return VSout;
}

