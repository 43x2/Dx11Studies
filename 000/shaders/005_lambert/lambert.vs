
// 頂点シェーダ
// Debug:   fxc /E vs_main /T vs_5_0 /Fh src/lambert.vs.h /Od /Vn g_vs_lambert /Zi ../000/shaders/005_lambert/lambert.vs
// Release: fxc /E vs_main /T vs_5_0 /Fh src/lambert.vs.h /O1 /Vn g_vs_lambert ../000/shaders/005_lambert/lambert.vs

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

	float4   diffusePosition;	// 光源の位置ベクトル

	float4   colorAmbient;		// 環境光の色
	float4   kAmbient;			// 環境光反射係数
	float4   colorDiffuse;		// 拡散反射光の色
	float4   kDiffuse;			// 拡散反射光反射係数
}


VS_OUT vs_main( VS_IN VSin )
{
	// 頂点色
	float4 vertexColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );

	// ランバート拡散反射
	float4 lambert = min( vertexColor * colorAmbient * kAmbient +
	                      vertexColor * colorDiffuse * kDiffuse * max( dot( VSin.normal, normalize( diffusePosition.xyz ) ), 0.0f ), 1.0f );


	VS_OUT VSout;

	float4x4 wvp = mul( mul( projection, view ), world );

	VSout.position = mul( wvp, float4( VSin.position, 1.0f ) );
	VSout.color    = float4( lambert.rgb, 1.0f );

	return VSout;
}

