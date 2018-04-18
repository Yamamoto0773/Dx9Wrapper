float4x4 proj;
float4x4 world;

float4 color;

texture tex;
texture tex2;

float uv_left;
float uv_top;
float uv_width;
float uv_height;

sampler smp_tex = sampler_state {
	texture = <tex>;
};
sampler smp_Tex2 = sampler_state {
	texture = <tex2>;
};


struct VS_IN {
	float3 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};




/////////////////////////////////////////////////////////
// 頂点シェーダ
/////////////////////////////////////////////////////////

// テクスチャを利用する場合(UV計算あり)の頂点シェーダ
VS_OUT vs_texWithUV( VS_IN In ) {
	VS_OUT Out = (VS_OUT)0;
	 
	Out.pos = mul( float4(In.pos, 1.0f), world );
	Out.pos = mul( Out.pos, proj );
	Out.uv  = In.uv * float2(uv_width, uv_height) + float2(uv_left, uv_top);
	 
	return Out;
}


// テクスチャを利用する場合の頂点シェーダ
VS_OUT vs_tex( VS_IN In ) {
	VS_OUT Out = (VS_OUT)0;
	 
	Out.pos = mul( float4(In.pos, 1.0f), world );
	Out.pos = mul( Out.pos, proj );
	Out.uv  = In.uv;
	 
	return Out;
}



// テクスチャを利用しない場合の頂点シェーダ
float4 vs_noTex( float3 pos : POSITION ) : POSITION {
	float4 Out;

	Out = mul( float4(pos, 1.0f), world );
	Out = mul( Out, proj );

	return Out;
}





//////////////////////////////////////////////////////////
// ピクセルシェーダ
/////////////////////////////////////////////////////////


// テクスチャサンプリングのみ
float4 ps_tex(VS_OUT In) : COLOR0 {
	return tex2D( smp_tex, In.uv );
}


// テクスチャサンプリングX指定色
float4 ps_tex_color(VS_OUT In) : COLOR0 {
	float4 texColor = tex2D( smp_tex, In.uv );
	texColor*=color;

	return texColor;
}


// 指定色のみ
float4 ps_color() : COLOR0 {
	return color;
}


// 指定色の透明度X(1-テクスチャの透明度)
float4 ps_color_invTexAlpha(VS_OUT In) : COLOR0 {
	float4 alphaTex = tex2D( smp_tex, In.uv );
	float4 outColor;
	outColor = color;
	outColor.a = color.a * (1.0 - alphaTex.a);

	return outColor;
}


// 指定色の透明度Xテクスチャの透明度
float4 ps_color_texAlpha(VS_OUT In) : COLOR0 {
	float4 alphaTex = tex2D( smp_tex, In.uv );
	float4 outColor;
	outColor = color;
	outColor.a *= alphaTex.a;

	return outColor;
}



technique Tech {

	// 指定色で塗りつぶし
	pass p0 {
		VertexShader = compile vs_2_0 vs_noTex();
		PixelShader  = compile ps_2_0 ps_color();

		AlphaBlendEnable = true;
	}

	// テクスチャをただ描画
	pass p1 {
		VertexShader = compile vs_2_0 vs_tex();
		PixelShader  = compile ps_2_0 ps_tex();

		AlphaBlendEnable = true;
	}

	// テクスチャと指定色を掛け算
	pass p2 {
		VertexShader = compile vs_2_0 vs_tex();
		PixelShader  = compile ps_2_0 ps_tex_color();

		AlphaBlendEnable = true;
	}

	// テクスチャの透明度を元に，指定色で塗りつぶし
	pass p3 {
		VertexShader = compile vs_2_0 vs_tex();
		PixelShader  = compile ps_2_0 ps_color_texAlpha();

		AlphaBlendEnable = true;
	}

	// テクスチャの透明度の反転値を元に，指定色で塗りつぶし
	pass p4 {
		VertexShader = compile vs_2_0 vs_tex();
		PixelShader  = compile ps_2_0 ps_color_invTexAlpha();

		AlphaBlendEnable = true;
	}

	// テクスチャをUV指定で描画
	pass p5 {
		VertexShader = compile vs_2_0 vs_texWithUV();
		PixelShader  = compile ps_2_0 ps_tex_color();

		AlphaBlendEnable = true;
	}


	// テクスチャの透明度の反転値を元に，指定色で塗りつぶし
	pass p6 {
		VertexShader = compile vs_2_0 vs_texWithUV();
		PixelShader  = compile ps_2_0 ps_color_invTexAlpha();

		AlphaBlendEnable = true;
	}



}
