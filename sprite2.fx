float4x4 proj;
float4x4 world;

float4 color;

texture tex;
float uv_left;
float uv_top;
float uv_width;
float uv_height;

sampler smp = sampler_state {
    texture = <tex>;
};


struct VS_IN {
    float3 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct VS_OUT {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};


// 頂点シェーダ
VS_OUT vs_main( VS_IN In ) {
   VS_OUT Out = (VS_OUT)0;
   
   Out.pos = mul( float4(In.pos, 1.0f), world );
   Out.pos = mul( Out.pos, proj );
   Out.uv  = In.uv * float2(uv_width, uv_height) + float2(uv_left, uv_top);
   
   return Out;
}


// ピクセルシェーダ
float4 ps_main(VS_OUT In) : COLOR0 {
    float4 texColor = tex2D( smp, In.uv );
    texColor*=color;
    return texColor;
}


technique Tech {
    pass p0 {
        VertexShader = compile vs_2_0 vs_main();
        PixelShader  = compile ps_2_0 ps_main();
        
        AlphaBlendEnable = true;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
    }
}
