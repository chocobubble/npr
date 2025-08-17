# ToonForge 개발 Q&A

## 🚀 시작하기 관련

### Q: 플러그인과 프로젝트 모듈 중 어떤 것을 선택해야 하나요?
**A:** ToonForge는 플러그인으로 개발하는 것이 맞습니다.

**플러그인을 선택하는 이유:**
- ✅ **재사용성**: 여러 프로젝트에서 사용 가능
- ✅ **포트폴리오**: GitHub에서 독립적으로 관리
- ✅ **배포**: 마켓플레이스 출시 가능
- ✅ **모듈화**: 깔끔한 코드 구조

**프로젝트 모듈이 적합한 경우:**
- 특정 프로젝트에만 사용되는 기능
- 빠른 프로토타이핑이 필요한 경우

### Q: UE5 버전은 어떤 것을 사용해야 하나요?
**A:** UE5.3 이상을 권장합니다.

**이유:**
- 최신 렌더링 기능 (Lumen, Nanite) 안정화
- 개선된 RHI 시스템
- 더 나은 플러그인 지원
- 취업 시장에서 요구하는 최신 기술

### Q: C++ 경험이 부족한데 괜찮을까요?
**A:** 단계적으로 학습하면서 진행하면 됩니다.

**학습 순서:**
1. **Week 1**: 기본 C++ 클래스 구조 이해
2. **Week 2**: UE5 API 사용법 익히기
3. **Week 3**: 고급 기능 및 최적화

**도움이 되는 자료:**
- UE5 공식 C++ 튜토리얼
- 기존 플러그인 소스 코드 분석
- 커뮤니티 Q&A 활용

---

## 🏗️ 아키텍처 관련

### Q: Runtime 모듈과 Editor 모듈을 왜 분리하나요?
**A:** 성능과 배포 최적화를 위해서입니다.

**Runtime 모듈:**
- 게임 실행 시 필요한 코드
- 패키징된 게임에 포함됨
- 성능이 중요함

**Editor 모듈:**
- 개발 도구, UI 패널
- 에디터에서만 로드됨
- 패키징된 게임에서 제외됨

```cpp
// 잘못된 예 - Runtime에 에디터 코드
class TOONFORGE_API FToonForgeManager
{
    void ShowEditorPanel();  // ❌ Runtime에 에디터 기능
};

// 올바른 예 - 모듈 분리
// Runtime: 핵심 NPR 기능만
class TOONFORGE_API FToonForgeManager
{
    void ApplyToonShading();  // ✅ 게임에서 사용되는 기능
};

// Editor: UI 및 개발 도구
class TOONFORGEEEDITOR_API FToonForgeEditorManager
{
    void ShowEditorPanel();   // ✅ 에디터 전용 기능
};
```

### Q: 셰이더 파일은 어디에 두어야 하나요?
**A:** `Shaders/Private/` 폴더에 두고, 가상 경로로 접근합니다.

**폴더 구조:**
```
ToonForge/
└── Shaders/
    ├── Private/
    │   ├── ToonShading.usf      # 실제 파일 위치
    │   └── OutlineDetection.usf
    └── Public/
        └── NPRCommon.ush        # 공통 헤더
```

**셰이더에서 접근:**
```hlsl
// 가상 경로 사용
#include "/ToonForge/Private/ToonShading.usf"
#include "/ToonForge/Public/NPRCommon.ush"
```

**C++에서 셰이더 등록:**
```cpp
IMPLEMENT_GLOBAL_SHADER(FToonShadingPS, "/ToonForge/Private/ToonShading.usf", "MainPS", SF_Pixel);
```

### Q: 성능 모니터링은 어떻게 구현하나요?
**A:** RHI 쿼리와 통계 시스템을 활용합니다.

**GPU 시간 측정:**
```cpp
class FNPRPerformanceMonitor
{
    FRenderQueryRHIRef GPUTimer;
    
public:
    void BeginGPUTiming(FRHICommandListImmediate& RHICmdList)
    {
        RHICmdList.EndRenderQuery(GPUTimer);
    }
    
    void EndGPUTiming(FRHICommandListImmediate& RHICmdList)
    {
        RHICmdList.EndRenderQuery(GPUTimer);
    }
    
    float GetLastGPUTime()
    {
        uint64 Result = 0;
        if (RHIGetRenderQueryResult(GPUTimer, Result, false))
        {
            return Result * 1e-6f; // 나노초를 밀리초로
        }
        return 0.0f;
    }
};
```

---

## 🎨 렌더링 관련

### Q: 포스트 프로세싱 체인에 어떻게 NPR 효과를 추가하나요?
**A:** `FPostProcessing` 시스템을 확장하거나 커스텀 패스를 추가합니다.

**방법 1: 기존 체인 확장**
```cpp
// PostProcessing.cpp 수정 (엔진 수정 필요)
void FPostProcessing::Process(FRHICommandListImmediate& RHICmdList, const FViewInfo& View)
{
    // ... 기존 포스트 프로세싱 ...
    
    // NPR 효과 추가
    if (ShouldRenderNPREffects(View))
    {
        RenderNPREffects(RHICmdList, View);
    }
}
```

**방법 2: 커스텀 렌더 패스 (권장)**
```cpp
// 독립적인 NPR 렌더 패스
class FNPRRenderPass
{
public:
    static void Render(FRHICommandListImmediate& RHICmdList, const FViewInfo& View)
    {
        // NPR 효과들을 순차적으로 실행
        RenderToonShading(RHICmdList, View);
        RenderOutlines(RHICmdList, View);
        RenderWatercolor(RHICmdList, View);
    }
};
```

### Q: 셰이더 파라미터를 실시간으로 업데이트하려면?
**A:** Game Thread에서 Render Thread로 안전하게 데이터를 전달해야 합니다.

**올바른 방법:**
```cpp
// Game Thread에서 호출
void UpdateToonSteps(float NewSteps)
{
    // 값을 복사해서 전달 (중요!)
    ENQUEUE_RENDER_COMMAND(UpdateToonSteps)(
        [NewSteps](FRHICommandListImmediate& RHICmdList)
        {
            // Render Thread에서 실행
            GetGlobalToonSteps() = NewSteps;
        });
}
```

**잘못된 방법:**
```cpp
// ❌ 직접 접근 (크래시 위험)
void UpdateToonSteps(float NewSteps)
{
    GetGlobalToonSteps() = NewSteps;  // 스레드 안전하지 않음!
}
```

### Q: 여러 NPR 효과를 조합할 때 성능 최적화는?
**A:** 렌더 타겟 재사용과 배치 처리를 활용합니다.

**렌더 타겟 재사용:**
```cpp
class FNPRRenderTargetManager
{
    TRefCountPtr<IPooledRenderTarget> TempRT1, TempRT2;
    
public:
    void ExecuteNPRChain(FRHICommandListImmediate& RHICmdList)
    {
        // Input: SceneColor
        // Output: TempRT1
        RenderToonShading(RHICmdList, SceneColor, TempRT1);
        
        // Input: TempRT1
        // Output: TempRT2  
        RenderOutlines(RHICmdList, TempRT1, TempRT2);
        
        // Input: TempRT2
        // Output: SceneColor (최종)
        RenderWatercolor(RHICmdList, TempRT2, SceneColor);
        
        // TempRT1, TempRT2는 자동으로 풀로 반환됨
    }
};
```

---

## 🎮 에디터 통합 관련

### Q: Slate UI가 복잡한데, 간단한 방법은 없나요?
**A:** 기본 위젯들을 조합해서 단계적으로 구현하면 됩니다.

**간단한 NPR 패널 예제:**
```cpp
class SNPRControlPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SNPRControlPanel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        ChildSlot
        [
            SNew(SVerticalBox)
            
            // 제목
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Title", "NPR Control Panel"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
            ]
            
            // 툰 단계 슬라이더
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.3f)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("ToonSteps", "Toon Steps"))
                ]
                + SHorizontalBox::Slot()
                .FillWidth(0.7f)
                [
                    SNew(SSlider)
                    .MinValue(2.0f)
                    .MaxValue(8.0f)
                    .Value(4.0f)
                    .OnValueChanged(this, &SNPRControlPanel::OnToonStepsChanged)
                ]
            ]
        ];
    }

private:
    void OnToonStepsChanged(float NewValue)
    {
        // 툰 단계 업데이트
        FToonForgeManager::Get().SetToonSteps(NewValue);
    }
};
```

### Q: 에디터 메뉴에 NPR 패널을 어떻게 추가하나요?
**A:** `FLevelEditorModule`의 메뉴 확장 시스템을 사용합니다.

```cpp
void FToonForgeEditorModule::RegisterMenus()
{
    // 1. 탭 등록
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        TEXT("NPRControlPanel"),
        FOnSpawnTab::CreateRaw(this, &FToonForgeEditorModule::SpawnNPRTab)
    );
    
    // 2. 메뉴 항목 추가
    FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    
    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
    MenuExtender->AddMenuExtension(
        "WindowLayout",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& Builder)
        {
            Builder.AddMenuEntry(
                LOCTEXT("OpenNPRPanel", "NPR Control Panel"),
                LOCTEXT("OpenNPRPanelTooltip", "Open the NPR Control Panel"),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([]()
                {
                    FGlobalTabmanager::Get()->TryInvokeTab(TEXT("NPRControlPanel"));
                }))
            );
        })
    );
    
    LevelEditor.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}
```

---

## 🔧 최적화 관련

### Q: NPR 효과가 너무 느린데 어떻게 최적화하나요?
**A:** 단계적으로 병목 지점을 찾아 최적화합니다.

**1단계: 프로파일링**
```cpp
// GPU 시간 측정
SCOPED_GPU_STAT(RHICmdList, ToonShading);
RenderToonShading(RHICmdList);

// 결과 확인: stat gpu 명령어 사용
```

**2단계: 셰이더 최적화**
```hlsl
// 비효율적인 코드
float4 MainPS(FScreenVertexOutput Input) : SV_Target0
{
    // 매 픽셀마다 복잡한 계산
    for(int i = 0; i < 100; i++)
    {
        // 반복 계산
    }
}

// 최적화된 코드
float4 MainPS(FScreenVertexOutput Input) : SV_Target0
{
    // 미리 계산된 룩업 테이블 사용
    float3 Result = LookupTable.Sample(Sampler, Input.UV).rgb;
    return float4(Result, 1.0f);
}
```

**3단계: 렌더 타겟 최적화**
```cpp
// 해상도 스케일링
float ResolutionScale = GetNPRResolutionScale();  // 0.5f ~ 1.0f
FIntPoint NPRSize = FIntPoint(
    ViewSize.X * ResolutionScale,
    ViewSize.Y * ResolutionScale
);

// 낮은 해상도로 NPR 렌더링 후 업스케일
```

### Q: 메모리 사용량이 너무 많은데 어떻게 줄이나요?
**A:** 렌더 타겟 풀링과 포맷 최적화를 사용합니다.

**렌더 타겟 풀링:**
```cpp
class FNPRRenderTargetPool
{
    TMap<uint32, TArray<TRefCountPtr<IPooledRenderTarget>>> Pool;
    
public:
    TRefCountPtr<IPooledRenderTarget> AcquireRT(FIntPoint Size, EPixelFormat Format)
    {
        uint32 Key = GetTypeHash(Size) ^ GetTypeHash(Format);
        
        if (auto* RTArray = Pool.Find(Key))
        {
            if (RTArray->Num() > 0)
            {
                return RTArray->Pop();  // 재사용
            }
        }
        
        // 새로 생성
        return CreateNewRT(Size, Format);
    }
    
    void ReleaseRT(TRefCountPtr<IPooledRenderTarget> RT, FIntPoint Size, EPixelFormat Format)
    {
        uint32 Key = GetTypeHash(Size) ^ GetTypeHash(Format);
        Pool.FindOrAdd(Key).Add(RT);  // 풀로 반환
    }
};
```

**포맷 최적화:**
```cpp
// 메모리 사용량 비교 (1920x1080 기준)
PF_FloatRGBA    // 32MB (16bit * 4채널)
PF_A2B10G10R10  // 8MB  (10bit * 3채널 + 2bit)
PF_R8G8B8A8     // 8MB  (8bit * 4채널)

// NPR에서는 높은 정밀도가 불필요한 경우가 많음
EPixelFormat GetOptimalNPRFormat()
{
    return PF_R8G8B8A8;  // 대부분의 NPR 효과에 충분
}
```

---

## 🐛 트러블슈팅 관련

### Q: 셰이더 컴파일 에러가 계속 발생합니다.
**A:** 단계적으로 문제를 해결해보세요.

**1단계: 기본 셰이더 테스트**
```hlsl
// 최소한의 셰이더로 시작
float4 MainPS(FScreenVertexOutput Input) : SV_Target0
{
    return float4(1, 0, 0, 1);  // 빨간색 출력
}
```

**2단계: 로그 확인**
```
// 에디터 콘솔에서
log LogShaders VeryVerbose

// 또는 Output Log 창에서 LogShaders 필터링
```

**3단계: 일반적인 오류들**
```hlsl
// ❌ 잘못된 include 경로
#include "Common.ush"  // 절대 경로 사용해야 함

// ✅ 올바른 include 경로  
#include "/Engine/Private/Common.ush"

// ❌ 잘못된 함수 이름
float4 MainPixelShader(...)  // 함수명이 C++과 일치하지 않음

// ✅ 올바른 함수 이름
float4 MainPS(...)  // IMPLEMENT_GLOBAL_SHADER에서 지정한 이름
```

### Q: 플러그인이 로드되지 않습니다.
**A:** 의존성과 모듈 설정을 확인해보세요.

**체크리스트:**
1. **`.uplugin` 파일 확인**
```json
{
    "Modules": [
        {
            "Name": "ToonForge",
            "Type": "Runtime",
            "LoadingPhase": "PostConfigInit"  // 적절한 로딩 단계
        }
    ]
}
```

2. **Build.cs 파일 의존성 확인**
```cpp
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core",
    "CoreUObject", 
    "Engine",
    "RenderCore",  // 렌더링 관련 필수
    "Renderer",    // 렌더링 관련 필수
    "RHI"          // RHI 관련 필수
});
```

3. **모듈 매크로 확인**
```cpp
// 헤더 파일에서
class TOONFORGE_API FToonForgeModule : public IModuleInterface
//     ^^^^^^^^^^^^^ 모듈 API 매크로 필수

// 구현 파일에서
IMPLEMENT_MODULE(FToonForgeModule, ToonForge)
//                                 ^^^^^^^^^^ 모듈 이름 일치 확인
```

### Q: 에디터에서 NPR 효과가 보이지 않습니다.
**A:** 렌더링 파이프라인 연결을 확인해보세요.

**디버깅 단계:**
1. **로그 확인**
```cpp
UE_LOG(LogToonForge, Warning, TEXT("NPR Effect Rendering Started"));
// 로그가 출력되는지 확인
```

2. **간단한 색상 출력 테스트**
```hlsl
float4 MainPS(FScreenVertexOutput Input) : SV_Target0
{
    return float4(0, 1, 0, 1);  // 초록색으로 화면이 바뀌는지 확인
}
```

3. **포스트 프로세싱 활성화 확인**
```cpp
// 프로젝트 설정에서 Post Processing 활성화 확인
// 또는 콘솔 명령어로 확인
showflag.PostProcessing 1
```

---

## 📚 학습 및 개발 팁

### Q: UE5 소스 코드를 어떻게 활용해야 하나요?
**A:** 기존 구현을 참고해서 패턴을 학습하세요.

**유용한 참고 파일들:**
```
Engine/Source/Runtime/Renderer/Private/
├── PostProcess/
│   ├── PostProcessing.cpp           # 포스트 프로세싱 메인 로직
│   ├── PostProcessTonemap.cpp       # 톤 매핑 구현 참고
│   └── PostProcessBloom.cpp         # 블룸 효과 구현 참고
├── SceneRendering.cpp               # 씬 렌더링 메인 로직
└── DeferredShadingRenderer.cpp      # 디퍼드 렌더링 구현
```

**학습 방법:**
1. 비슷한 기능의 기존 구현 찾기
2. 클래스 구조와 함수 호출 흐름 파악
3. 핵심 로직만 추출해서 NPR에 적용

### Q: 개발 중 막힐 때는 어떻게 하나요?
**A:** 체계적으로 문제를 해결해보세요.

**문제 해결 순서:**
1. **로그 확인**: 에러 메시지나 경고 확인
2. **단순화**: 복잡한 코드를 간단하게 만들어 테스트
3. **참고 자료**: UE5 소스 코드나 문서 검색
4. **커뮤니티**: Discord, Reddit, 포럼에서 질문
5. **휴식**: 잠시 쉬고 다른 관점에서 접근

**효과적인 질문 방법:**
- 구체적인 에러 메시지 포함
- 시도해본 해결 방법 명시
- 최소한의 재현 가능한 코드 제공

이 Q&A를 통해 개발 중 마주칠 수 있는 대부분의 문제들을 해결할 수 있을 것입니다!
