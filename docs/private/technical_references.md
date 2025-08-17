# ToonForge 기술 레퍼런스 및 학습 자료

## 📚 핵심 학습 자료

### 언리얼 엔진 5 공식 문서
- **[Rendering and Graphics](https://docs.unrealengine.com/5.3/en-US/rendering-and-graphics-in-unreal-engine/)** - 렌더링 시스템 전체 개요
- **[RHI (Render Hardware Interface)](https://docs.unrealengine.com/5.3/en-US/render-hardware-interface-in-unreal-engine/)** - 하드웨어 추상화 계층
- **[Shader Development](https://docs.unrealengine.com/5.3/en-US/shader-development-in-unreal-engine/)** - 셰이더 개발 가이드
- **[Post Process Effects](https://docs.unrealengine.com/5.3/en-US/post-process-effects-in-unreal-engine/)** - 포스트 프로세싱 구현
- **[Plugin Development](https://docs.unrealengine.com/5.3/en-US/plugins-in-unreal-engine/)** - 플러그인 개발 가이드

### NPR 렌더링 기술 자료
- **[Real-Time Non-Photorealistic Rendering](https://www.cs.utah.edu/~shirley/papers/rtNPR.pdf)** - NPR 기초 이론
- **[Toon Shading Techniques](https://developer.nvidia.com/gpugems/gpugems/part-iii-materials/chapter-19-image-based-lighting)** - 툰 셰이딩 구현
- **[Edge Detection in Real-Time](https://www.shadertoy.com/view/Mdf3zr)** - 실시간 엣지 검출
- **[Watercolor Rendering](https://graphics.stanford.edu/papers/watercolor/)** - 수채화 렌더링 연구

---

## 🔧 필수 UE5 API 레퍼런스

### 렌더링 관련 클래스
```cpp
// 핵심 렌더링 클래스들
class FRenderResource;           // GPU 리소스 기본 클래스
class FGlobalShader;            // 전역 셰이더 기본 클래스
class FMaterialShader;          // 머티리얼 셰이더 기본 클래스
class FRHICommandListImmediate; // RHI 명령 리스트
class IPooledRenderTarget;      // 렌더 타겟 풀링
class FSceneView;               // 뷰 정보
class FViewInfo;                // 확장된 뷰 정보
```

### 플러그인 관련 클래스
```cpp
// 플러그인 개발 핵심 클래스들
class IModuleInterface;         // 모듈 인터페이스
class FRenderingThread;         // 렌더링 스레드 관리
class ENQUEUE_RENDER_COMMAND;   // 렌더 명령 큐잉
class FSlateApplication;        // UI 시스템
class SCompoundWidget;          // Slate 위젯 기본 클래스
```

---

## 💻 자주 사용할 코드 패턴

### 1. 플러그인 모듈 기본 구조
```cpp
// ToonForgeModule.h
#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToonForgeModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    /** 모듈 인스턴스 가져오기 */
    static FToonForgeModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FToonForgeModule>("ToonForge");
    }
    
    /** 모듈이 로드되었는지 확인 */
    static bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("ToonForge");
    }

private:
    /** 내부 매니저 클래스들 */
    TUniquePtr<class FToonForgeManager> Manager;
};
```

### 2. 렌더 리소스 생명주기 관리
```cpp
// GPU 리소스 안전한 관리 패턴
class TOONFORGE_API FMyRenderResource : public FRenderResource
{
public:
    FMyRenderResource(const TArray<uint8>& InData) : Data(InData) {}
    
    // Render Thread에서 GPU 리소스 초기화
    virtual void InitRHI() override
    {
        // 텍스처 생성
        FRHIResourceCreateInfo CreateInfo(TEXT("MyTexture"));
        TextureRHI = RHICreateTexture2D(
            Width, Height, PF_R8G8B8A8, 1, 1,
            TexCreate_ShaderResource, CreateInfo
        );
        
        // 데이터 업로드
        uint32 DestStride;
        void* DestData = RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false);
        FMemory::Memcpy(DestData, Data.GetData(), Data.Num());
        RHIUnlockTexture2D(TextureRHI, 0, false);
    }
    
    // Render Thread에서 GPU 리소스 해제
    virtual void ReleaseRHI() override
    {
        TextureRHI.SafeRelease();
    }

private:
    TArray<uint8> Data;              // CPU 데이터
    FTexture2DRHIRef TextureRHI;     // GPU 텍스처
    uint32 Width = 256, Height = 256;
};
```

### 3. 셰이더 클래스 정의 패턴
```cpp
// 글로벌 셰이더 정의 패턴
class FToonShadingPS : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FToonShadingPS);
    SHADER_USE_PARAMETER_STRUCT(FToonShadingPS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        // 텍스처 파라미터
        SHADER_PARAMETER_TEXTURE(Texture2D, SceneColorTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, SceneColorSampler)
        
        // 스칼라 파라미터
        SHADER_PARAMETER(float, ToonSteps)
        SHADER_PARAMETER(float, ShadowIntensity)
        SHADER_PARAMETER(FVector3f, ShadowColor)
        
        // 렌더 타겟
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
};

// 구현 파일에서
IMPLEMENT_GLOBAL_SHADER(FToonShadingPS, "/ToonForge/ToonShading.usf", "MainPS", SF_Pixel);
```

### 4. 렌더 명령 큐잉 패턴
```cpp
// Game Thread에서 Render Thread로 안전한 데이터 전달
void UpdateToonShadingParameters(const FToonShadingParams& NewParams)
{
    // 데이터를 복사해서 Render Thread로 전달
    ENQUEUE_RENDER_COMMAND(UpdateToonShading)(
        [NewParams](FRHICommandListImmediate& RHICmdList)
        {
            // 이 코드는 Render Thread에서 실행됨
            GetGlobalToonShadingParameters() = NewParams;
        });
}
```

### 5. 성능 측정 패턴
```cpp
// GPU 타이밍 측정
class FGPUTimer
{
public:
    void BeginTiming(FRHICommandListImmediate& RHICmdList, const FString& Name)
    {
        if (!TimerQueries.Contains(Name))
        {
            TimerQueries.Add(Name, RHICreateRenderQuery(RQT_AbsoluteTime));
        }
        
        RHICmdList.EndRenderQuery(TimerQueries[Name]);
    }
    
    void EndTiming(FRHICommandListImmediate& RHICmdList, const FString& Name)
    {
        if (TimerQueries.Contains(Name))
        {
            RHICmdList.EndRenderQuery(TimerQueries[Name]);
        }
    }
    
    float GetLastTime(const FString& Name) const
    {
        if (TimerQueries.Contains(Name))
        {
            uint64 Result = 0;
            if (RHIGetRenderQueryResult(TimerQueries[Name], Result, false))
            {
                return Result * 1e-6f; // 마이크로초를 밀리초로 변환
            }
        }
        return 0.0f;
    }

private:
    TMap<FString, FRenderQueryRHIRef> TimerQueries;
};
```

---

## 🎨 HLSL 셰이더 코드 패턴

### 1. 기본 포스트 프로세싱 셰이더 구조
```hlsl
// ToonShading.usf
#include "/Engine/Private/Common.ush"
#include "/Engine/Private/PostProcessCommon.ush"

// 파라미터 정의
float ToonSteps;
float ShadowIntensity;
float3 ShadowColor;

// 입력 텍스처
Texture2D SceneColorTexture;
SamplerState SceneColorSampler;

// 메인 픽셀 셰이더
float4 MainPS(FScreenVertexOutput Input) : SV_Target0
{
    float2 UV = Input.UV;
    float3 SceneColor = SceneColorTexture.Sample(SceneColorSampler, UV).rgb;
    
    // 툰 셰이딩 로직
    float3 QuantizedColor = floor(SceneColor * ToonSteps) / ToonSteps;
    
    return float4(QuantizedColor, 1.0f);
}
```

### 2. 컴퓨트 셰이더 패턴
```hlsl
// OutlineCompute.usf
#include "/Engine/Private/Common.ush"

// 컴퓨트 셰이더 파라미터
float2 ScreenSize;
float OutlineThickness;

// 입출력 텍스처
Texture2D InputTexture;
RWTexture2D<float4> OutputTexture;

// 컴퓨트 셰이더 메인 함수
[numthreads(8, 8, 1)]
void MainCS(uint3 id : SV_DispatchThreadID)
{
    float2 UV = (float2(id.xy) + 0.5f) / ScreenSize;
    
    // 경계 체크
    if (id.x >= (uint)ScreenSize.x || id.y >= (uint)ScreenSize.y)
        return;
    
    // 엣지 검출 로직
    float4 Result = InputTexture.SampleLevel(PointSampler, UV, 0);
    
    OutputTexture[id.xy] = Result;
}
```

---

## 🛠️ 개발 도구 설정

### Visual Studio 설정
```json
// .vscode/settings.json (VS Code 사용 시)
{
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/Source/**",
        "C:/Program Files/Epic Games/UE_5.3/Engine/Source/**"
    ],
    "C_Cpp.default.defines": [
        "WITH_EDITOR=1",
        "UE_BUILD_DEVELOPMENT=1"
    ]
}
```

### Git 설정
```gitignore
# .gitignore for UE5 projects
Binaries/
Intermediate/
DerivedDataCache/
Saved/
.vs/
*.sln
*.suo
*.sdf
*.opensdf
*.udb
*.opendb
```

### 빌드 스크립트
```batch
@echo off
REM BuildToonForge.bat
echo Building ToonForge Plugin...

REM 언리얼 빌드 툴 경로
set UBT="C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

REM 프로젝트 빌드
%UBT% ToonForgeDemo Win64 Development -Project="ToonForgeDemo.uproject" -WaitMutex -FromMsBuild

echo Build completed!
pause
```

---

## 🐛 디버깅 및 트러블슈팅

### 자주 발생하는 문제들

#### 1. 셰이더 컴파일 에러
```
문제: "Shader compilation failed"
원인: HLSL 문법 오류, 잘못된 include 경로
해결: 
- 셰이더 로그 확인 (Output Log → LogShaders)
- 문법 검사 도구 사용
- 단계적 구현으로 문제 지점 특정
```

#### 2. 렌더 타겟 생성 실패
```
문제: "Failed to create render target"
원인: 메모리 부족, 잘못된 포맷 지정
해결:
- 메모리 사용량 확인
- 포맷 호환성 검사
- 크기 제한 확인
```

#### 3. 크래시 및 메모리 누수
```
문제: 에디터 크래시, 메모리 누수
원인: 잘못된 포인터 사용, 리소스 해제 누락
해결:
- 스마트 포인터 사용
- RAII 패턴 적용
- 메모리 프로파일러 활용
```

### 디버깅 도구
- **Visual Studio Debugger**: C++ 코드 디버깅
- **RenderDoc**: GPU 디버깅 및 프로파일링
- **Unreal Insights**: 성능 분석
- **Memory Profiler**: 메모리 사용량 분석

---

## 📖 추가 학습 리소스

### 온라인 강의
- **[Unreal Engine 5 Rendering](https://www.unrealengine.com/en-US/onlinelearning)** - 공식 온라인 학습
- **[Real-Time Rendering Course](https://www.realtimerendering.com/)** - 실시간 렌더링 이론
- **[Shader Programming](https://learnopengl.com/Advanced-Lighting/Advanced-Lighting)** - 셰이더 프로그래밍

### 커뮤니티
- **[Unreal Engine Discord](https://discord.gg/unreal-engine)** - 실시간 질문/답변
- **[r/unrealengine](https://reddit.com/r/unrealengine)** - Reddit 커뮤니티
- **[Unreal Engine Forums](https://forums.unrealengine.com/)** - 공식 포럼

### 참고 프로젝트
- **[UE5 Samples](https://github.com/EpicGames/UnrealEngine)** - 공식 샘플 프로젝트
- **[Community NPR Projects](https://github.com/topics/unreal-engine-npr)** - 커뮤니티 NPR 프로젝트

---

## 🎯 학습 체크리스트

### Week 1 필수 학습
- [ ] UE5 플러그인 구조 이해
- [ ] RHI 기본 사용법
- [ ] 포스트 프로세싱 파이프라인
- [ ] 기본 셰이더 작성

### Week 2 필수 학습
- [ ] 고급 셰이더 기법
- [ ] 성능 최적화 기법
- [ ] Slate UI 시스템
- [ ] 에디터 확장

### Week 3 필수 학습
- [ ] 메모리 관리 최적화
- [ ] 크로스 플랫폼 호환성
- [ ] 프로파일링 및 디버깅
- [ ] 포트폴리오 문서화

---

이 레퍼런스를 통해 **체계적이고 효율적인 학습**을 진행하여 ToonForge 개발을 성공적으로 완료할 수 있습니다!
