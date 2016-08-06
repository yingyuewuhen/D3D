//----------------------------------------------------------------------------------------------------------
//    Technique:     Water(水面)
//    
//    作者:        www.riemers.net
//
//  来自：      http://shiba.hpe.sh.cn/jiaoyanzu/WULI/soft/xna.aspx?classId=4(上海八中物理组Xna游戏开发专栏)
//
//    --------------------------------------------------------------------------------------------------------

shared uniform extern float4x4    gProjection : PROJECTION;   // 共享的投影矩阵
shared uniform extern float     gTime;                        // 共享的时间变量
shared uniform extern int        gTotalLights;                // 共享的光源数量

// 包含光源数据的结构
struct Light
{
    float enabled;      //光源是否打开    
    float lightType;    //光源类型
    float3 color;        //光源颜色
    float3 position;    //光源位置
    float3 direction;    //光线方向
    float4 spotData;    //四个分量分别保存range,falloff,theta,phi数据
};

//光源数组
shared Light gLights[8];

shared uniform extern float3    gCameraPos;                           // 相机位置 
shared uniform extern float4x4    gView : VIEW;                       // 视矩阵

uniform extern float4x4 gReflectionView;                   // 反射视矩阵
uniform extern float4x4    gWorld : WORLD;                       // 世界矩阵

uniform extern float gWaveHeight;                           // 振幅
uniform extern float gWindForce;                           // 风力大小，即波速
uniform extern float3 gWindDirection;                       // 风向，即水流方向

uniform extern texture gTexture1;                         // 水面的凹凸贴图
uniform extern float    gTexture1UVTile;                  // 凹凸纹理的平铺次数
sampler BumpMapSampler = sampler_state { 
    texture = <gTexture1> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;
};

uniform extern texture gReflectionMap;                   //反射贴图
sampler ReflectionSampler = sampler_state { 
    texture = <gReflectionMap> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;
};

uniform extern texture gRefractionMap;                   //折射贴图
sampler RefractionSampler = sampler_state { 
    texture = <gRefractionMap> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;
};

struct VS_OUTPUT
{
    float4 Position                  : POSITION;
    float4 ReflectionMapSamplingPos  : TEXCOORD1;
    float2 BumpMapSamplingPos        : TEXCOORD2;
    float4 RefractionMapSamplingPos  : TEXCOORD3;
    float4 WorldPosition             : TEXCOORD4;
}; 


VS_OUTPUT WaterVS(float4 inPos : POSITION, float2 inTex: TEXCOORD)
{    
    VS_OUTPUT Output = (VS_OUTPUT)0;

    float4x4 preViewProjection = mul (gView, gProjection);
    float4x4 preWorldViewProjection = mul (gWorld, preViewProjection);
    Output.Position = mul(inPos, preWorldViewProjection);
    Output.WorldPosition = mul(inPos, gWorld);
    
    // 计算反射纹理的采样坐标
    float4x4 preReflectionViewProjection = mul (gReflectionView, gProjection);
    float4x4 preWorldReflectionViewProjection = mul (gWorld, preReflectionViewProjection);    
    Output.ReflectionMapSamplingPos = mul(inPos, preWorldReflectionViewProjection);          
    
    // 设置纹理的采样坐标
    Output.RefractionMapSamplingPos = mul(inPos, preWorldViewProjection);            
    
    // 归一化水流方向
    float3 windDir = normalize(gWindDirection);    
    // 获取垂直于水流的方向
    float3 perpDir = cross(gWindDirection, float3(0,1,0));
    
    // 获取经水流方向修正的纹理uv坐标
    float ydot = dot(inTex, gWindDirection.xz);
    float xdot = dot(inTex, perpDir.xz);
    float2 moveVector = float2(xdot, ydot);
    // 让纹理的v坐标随时间移动
    moveVector.y += gTime*gWindForce;    
    // 获取最终的凹凸纹理采样坐标
    Output.BumpMapSamplingPos = moveVector*gTexture1UVTile;
    
    return Output;
}

float4 WaterPS(VS_OUTPUT Input):COLOR0
{
    // 采样凹凸纹理颜色
    float4 bumpColor = tex2D(BumpMapSampler, Input.BumpMapSamplingPos);
    float2 perturbation = gWaveHeight*(bumpColor.rg - 0.5f)*2.0f;
    
    // 将反射贴图采样坐标从2D屏幕空间映射到纹理坐标
    float2 ProjectedReflectTexCoords;
    ProjectedReflectTexCoords.x = Input.ReflectionMapSamplingPos.x/Input.ReflectionMapSamplingPos.w/2.0f + 0.5f;
    ProjectedReflectTexCoords.y = -Input.ReflectionMapSamplingPos.y/Input.ReflectionMapSamplingPos.w/2.0f + 0.5f;        
    float2 perturbatedTexCoords = ProjectedReflectTexCoords + perturbation;
    float4 reflectiveColor = tex2D(ReflectionSampler, perturbatedTexCoords);
    
    // 将折射贴图采样坐标从2D屏幕空间映射到纹理坐标
    float2 ProjectedRefrTexCoords;
    ProjectedRefrTexCoords.x = Input.RefractionMapSamplingPos.x/Input.RefractionMapSamplingPos.w/2.0f + 0.5f;
    ProjectedRefrTexCoords.y = -Input.RefractionMapSamplingPos.y/Input.RefractionMapSamplingPos.w/2.0f + 0.5f;    
    float2 perturbatedRefrTexCoords = ProjectedRefrTexCoords + perturbation;    
    float4 refractiveColor = tex2D(RefractionSampler, perturbatedRefrTexCoords);
    
    // 从凹凸贴图获取法线向量
    float3 eyeVector = normalize(gCameraPos - Input.WorldPosition);
    float3 normalVector = (bumpColor.rbg-0.5f)*2.0f;
    
    // 计算菲涅尔系数，并根据这个系数混合反射和折射颜色
    float fresnelTerm = dot(eyeVector, normalVector);    
    float4 combinedColor = lerp(reflectiveColor, refractiveColor, fresnelTerm);
    
    // 在水面再添加蓝灰色让它变得“脏”一点
    float4 dullColor = float4(0.3f, 0.3f, 0.5f, 1.0f);
    
    // 将蓝灰色混合到最终颜色
    float4 color = lerp(combinedColor, dullColor, 0.2f);
    
    // 设置光源方向，为简化起见，只使用场景中的单向光方向
    float3 gLightDirection = float3(5, -1, -2);
    //----------------------------
    //    遍历所有光源
    //----------------------------   
    for(int i=0; i < gTotalLights; i++)
    {    
        
        //只处理可用的光源
        if(gLights[i].enabled&&gLights[i].lightType==0)
        {
             gLightDirection=gLights[i].direction;
        }
    }        
    
    // 添加水面的镜面反射颜色
    float3 reflectionVector = reflect(-gLightDirection, normalVector);    
    float specular = pow(dot(normalize(reflectionVector), normalize(eyeVector)), 1024);        
    color.rgb += specular;
    
    return color;
}

technique Water
{
    pass Pass0
    {
        //VertexShader = compile vs_3_0 WaterVS();
        PixelShader = compile ps_3_0 WaterPS();
    }
}