//----------------------------------------------------------------------------------------------------------
//    Technique:     Water(ˮ��)
//    
//    ����:        www.riemers.net
//
//  ���ԣ�      http://shiba.hpe.sh.cn/jiaoyanzu/WULI/soft/xna.aspx?classId=4(�Ϻ�����������Xna��Ϸ����ר��)
//
//    --------------------------------------------------------------------------------------------------------

shared uniform extern float4x4    gProjection : PROJECTION;   // �����ͶӰ����
shared uniform extern float     gTime;                        // �����ʱ�����
shared uniform extern int        gTotalLights;                // ����Ĺ�Դ����

// ������Դ���ݵĽṹ
struct Light
{
    float enabled;      //��Դ�Ƿ��    
    float lightType;    //��Դ����
    float3 color;        //��Դ��ɫ
    float3 position;    //��Դλ��
    float3 direction;    //���߷���
    float4 spotData;    //�ĸ������ֱ𱣴�range,falloff,theta,phi����
};

//��Դ����
shared Light gLights[8];

shared uniform extern float3    gCameraPos;                           // ���λ�� 
shared uniform extern float4x4    gView : VIEW;                       // �Ӿ���

uniform extern float4x4 gReflectionView;                   // �����Ӿ���
uniform extern float4x4    gWorld : WORLD;                       // �������

uniform extern float gWaveHeight;                           // ���
uniform extern float gWindForce;                           // ������С��������
uniform extern float3 gWindDirection;                       // ���򣬼�ˮ������

uniform extern texture gTexture1;                         // ˮ��İ�͹��ͼ
uniform extern float    gTexture1UVTile;                  // ��͹�����ƽ�̴���
sampler BumpMapSampler = sampler_state { 
    texture = <gTexture1> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;
};

uniform extern texture gReflectionMap;                   //������ͼ
sampler ReflectionSampler = sampler_state { 
    texture = <gReflectionMap> ; magfilter = LINEAR; minfilter = LINEAR; mipfilter=LINEAR; AddressU = mirror; AddressV = mirror;
};

uniform extern texture gRefractionMap;                   //������ͼ
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
    
    // ���㷴������Ĳ�������
    float4x4 preReflectionViewProjection = mul (gReflectionView, gProjection);
    float4x4 preWorldReflectionViewProjection = mul (gWorld, preReflectionViewProjection);    
    Output.ReflectionMapSamplingPos = mul(inPos, preWorldReflectionViewProjection);          
    
    // ��������Ĳ�������
    Output.RefractionMapSamplingPos = mul(inPos, preWorldViewProjection);            
    
    // ��һ��ˮ������
    float3 windDir = normalize(gWindDirection);    
    // ��ȡ��ֱ��ˮ���ķ���
    float3 perpDir = cross(gWindDirection, float3(0,1,0));
    
    // ��ȡ��ˮ����������������uv����
    float ydot = dot(inTex, gWindDirection.xz);
    float xdot = dot(inTex, perpDir.xz);
    float2 moveVector = float2(xdot, ydot);
    // �������v������ʱ���ƶ�
    moveVector.y += gTime*gWindForce;    
    // ��ȡ���յİ�͹�����������
    Output.BumpMapSamplingPos = moveVector*gTexture1UVTile;
    
    return Output;
}

float4 WaterPS(VS_OUTPUT Input):COLOR0
{
    // ������͹������ɫ
    float4 bumpColor = tex2D(BumpMapSampler, Input.BumpMapSamplingPos);
    float2 perturbation = gWaveHeight*(bumpColor.rg - 0.5f)*2.0f;
    
    // ��������ͼ���������2D��Ļ�ռ�ӳ�䵽��������
    float2 ProjectedReflectTexCoords;
    ProjectedReflectTexCoords.x = Input.ReflectionMapSamplingPos.x/Input.ReflectionMapSamplingPos.w/2.0f + 0.5f;
    ProjectedReflectTexCoords.y = -Input.ReflectionMapSamplingPos.y/Input.ReflectionMapSamplingPos.w/2.0f + 0.5f;        
    float2 perturbatedTexCoords = ProjectedReflectTexCoords + perturbation;
    float4 reflectiveColor = tex2D(ReflectionSampler, perturbatedTexCoords);
    
    // ��������ͼ���������2D��Ļ�ռ�ӳ�䵽��������
    float2 ProjectedRefrTexCoords;
    ProjectedRefrTexCoords.x = Input.RefractionMapSamplingPos.x/Input.RefractionMapSamplingPos.w/2.0f + 0.5f;
    ProjectedRefrTexCoords.y = -Input.RefractionMapSamplingPos.y/Input.RefractionMapSamplingPos.w/2.0f + 0.5f;    
    float2 perturbatedRefrTexCoords = ProjectedRefrTexCoords + perturbation;    
    float4 refractiveColor = tex2D(RefractionSampler, perturbatedRefrTexCoords);
    
    // �Ӱ�͹��ͼ��ȡ��������
    float3 eyeVector = normalize(gCameraPos - Input.WorldPosition);
    float3 normalVector = (bumpColor.rbg-0.5f)*2.0f;
    
    // ���������ϵ�������������ϵ����Ϸ����������ɫ
    float fresnelTerm = dot(eyeVector, normalVector);    
    float4 combinedColor = lerp(reflectiveColor, refractiveColor, fresnelTerm);
    
    // ��ˮ�����������ɫ������á��ࡱһ��
    float4 dullColor = float4(0.3f, 0.3f, 0.5f, 1.0f);
    
    // ������ɫ��ϵ�������ɫ
    float4 color = lerp(combinedColor, dullColor, 0.2f);
    
    // ���ù�Դ����Ϊ�������ֻʹ�ó����еĵ���ⷽ��
    float3 gLightDirection = float3(5, -1, -2);
    //----------------------------
    //    �������й�Դ
    //----------------------------   
    for(int i=0; i < gTotalLights; i++)
    {    
        
        //ֻ������õĹ�Դ
        if(gLights[i].enabled&&gLights[i].lightType==0)
        {
             gLightDirection=gLights[i].direction;
        }
    }        
    
    // ���ˮ��ľ��淴����ɫ
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