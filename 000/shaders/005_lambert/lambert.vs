
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
}


VS_OUT vs_main( VS_IN VSin )
{
	// 物体から光源方向への単位ベクトル
	float3 object_to_light = normalize( float3( 1.0f, 1.0f, -1.0f ) );
	// 環境明度
	float  intensityA = 0.2f;
	// 拡散反射による最大明度
	float  intensityD = 1.0f;
	// 環境反射係数
	float  kA = 1.0f;
	// 拡散反射の反射係数
	float  kD = 1.0f;

	// ランバート拡散反射
	float lambert = min( intensityA * kA + intensityD * kD * max( dot( VSin.normal, object_to_light ), 0.0f ), 1.0f );


	VS_OUT VSout;

	float4x4 wvp = mul( mul( projection, view ), world );

	VSout.position = mul( float4( VSin.position, 1.0f ), transpose( wvp ) );
	VSout.color    = lambert * float4( 1.0f, 1.0f, 1.0f, 1.0f );

	return VSout;
}

