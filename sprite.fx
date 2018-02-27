// ワールドビュー射影変換行列
float4x4 matWorldViewProj;
// テクスチャ
texture tex;
// 色情報
float4 color;


sampler smp = sampler_state{
   texture = <tex>;
};

// 入力
struct VS_IN {
   float3 pos : POSITION;     // 頂点位置
   float2 uv : TEXCOORD0;     // テクスチャ座標
};


// 出力
struct VS_OUT {
   float4 pos : POSITION;
   float2 uv : TEXCOORD0;
};



// 頂点シェーダ
VS_OUT vs_main(VS_IN In) {
   VS_OUT Out = (VS_OUT)0;

   // ローカル座標にある頂点をワールドビュー射影変換で一気にスクリーン座標にしてしまう
   Out.pos = mul( float4(In.pos, 1.0f), matWorldViewProj );
   // テクスチャ座標はそのまま出力
   Out.uv = In.uv;

   return Out;
}


// ピクセルシェーダ
float4 ps_main(VS_OUT In) : COLOR0 {
   float4 outColor;

   // テクスチャの色を取得
   float4 texColor = tex2D(smp, In.uv);
   // 指定色に変換
   outColor.r = color.r;
   outColor.g = color.g;
   outColor.b = color.b;
   outColor.a = texColor.a*color.a;

   return outColor;
}


technique BasicTec
{
   pass P0
   {
      VertexShader = compile vs_2_0 vs_main();
      PixelShader = compile ps_2_0 ps_main();
   }
}