# UE5 플러그인 개발 핵심 개념

## 📚 플러그인 시스템 이해

### 플러그인이란?
- **정의**: 언리얼 엔진의 기능을 확장하는 독립적인 모듈
- **장점**: 재사용 가능, 배포 용이, 프로젝트 독립적
- **구성**: 코드 모듈 + 에셋 + 설정 파일

### 플러그인 vs 프로젝트 모듈
```
플러그인:
✅ 여러 프로젝트에서 재사용 가능
✅ 마켓플레이스 배포 가능
✅ 독립적인 버전 관리
❌ 프로젝트별 커스터마이징 제한

프로젝트 모듈:
✅ 프로젝트에 특화된 기능
✅ 빠른 개발 및 테스트
❌ 재사용성 낮음
❌ 배포 어려움
```

---

## 🏗️ 모듈 시스템 이해

### 모듈 타입별 특징
```cpp
// Runtime Module - 게임 실행 시 로드
class TOONFORGE_API FToonForgeModule : public IModuleInterface
{
    // 게임 로직, 렌더링 코드
    // 패키징된 게임에 포함됨
};

// Editor Module - 에디터에서만 로드  
class TOONFORGEEEDITOR_API FToonForgeEditorModule : public IModuleInterface
{
    // 에디터 UI, 개발 도구
    // 패키징된 게임에 포함되지 않음
};

// Developer Module - 개발 중에만 로드
class TOONFORGEDEVELOPER_API FToonForgeDeveloperModule : public IModuleInterface
{
    // 디버깅 도구, 프로파일링
    // 개발 빌드에서만 사용
};
```

### 모듈 로딩 단계
```
1. PreDefault     - 엔진 초기화 이전
2. Default        - 기본 로딩 단계
3. PostDefault    - 기본 모듈 로드 후
4. PreLoadingScreen - 로딩 화면 이전
5. PostConfigInit - 설정 초기화 후 ⭐ (ToonForge 사용)
6. PostEngineInit - 엔진 초기화 후 ⭐ (ToonForgeEditor 사용)
7. None           - 수동 로드
```

---

## 🎨 렌더링 시스템 통합

### RHI (Render Hardware Interface) 이해
```cpp
// RHI는 다양한 그래픽 API를 추상화
/*
Application Code
       ↓
   Unreal RHI
       ↓
┌──────┼──────┼──────┐
│ D3D11│ D3D12│Vulkan│ Metal
└──────┴──────┴──────┘
*/

// 주요 RHI 클래스들
FRHICommandListImmediate  // GPU 명령 기록
FRHITexture2D            // 2D 텍스처
FRHIBuffer               // 버퍼 (Vertex, Index, Uniform)
FRHIShader               // 셰이더 객체
FRHIRenderPassInfo       // 렌더 패스 정보
```

### 멀티스레드 렌더링
```cpp
// 3개의 주요 스레드
Game Thread:    // 게임 로직, 입력 처리
- Tick() 함수 실행
- 렌더링 명령 생성
- ENQUEUE_RENDER_COMMAND 사용

Render Thread:  // 렌더링 로직
- 씬 데이터 처리
- 드로우콜 생성
- RHI 명령 생성

RHI Thread:     // 하드웨어 호출
- GPU 드라이버 호출
- 실제 렌더링 실행
```

### 안전한 스레드 간 통신
```cpp
// Game Thread에서 Render Thread로 데이터 전달
void UpdateNPRSettings(const FNPRSettings& NewSettings)
{
    // 데이터를 복사해서 전달 (중요!)
    ENQUEUE_RENDER_COMMAND(UpdateNPRSettings)(
        [NewSettings](FRHICommandListImmediate& RHICmdList)
        {
            // 이 코드는 Render Thread에서 실행
            GetGlobalNPRSettings() = NewSettings;
        });
}

// 잘못된 예 - 포인터 전달 (위험!)
ENQUEUE_RENDER_COMMAND(BadExample)(
    [&NewSettings](FRHICommandListImmediate& RHICmdList)  // ❌ 참조 캡처
    {
        // NewSettings가 이미 소멸되었을 수 있음!
    });
```

---

## 🎯 셰이더 시스템 이해

### 셰이더 클래스 계층
```cpp
FShader                    // 모든 셰이더의 기본 클래스
├── FGlobalShader         // 전역 셰이더 (머티리얼 독립적)
│   ├── FPostProcessVS    // 포스트 프로세싱 버텍스 셰이더
│   └── FMyCustomPS       // 커스텀 픽셀 셰이더
├── FMaterialShader       // 머티리얼 기반 셰이더
└── FMeshMaterialShader   // 메시 + 머티리얼 셰이더
```

### 셰이더 파라미터 시스템
```cpp
// 현대적인 방식 (UE5 권장)
class FMyShaderPS : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FMyShaderPS);
    SHADER_USE_PARAMETER_STRUCT(FMyShaderPS, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER(float, MyFloat)
        SHADER_PARAMETER(FVector3f, MyVector)
        SHADER_PARAMETER_TEXTURE(Texture2D, MyTexture)
        SHADER_PARAMETER_SAMPLER(SamplerState, MySampler)
        RENDER_TARGET_BINDING_SLOTS()
    END_SHADER_PARAMETER_STRUCT()
};

// 사용법
FMyShaderPS::FParameters* Parameters = GraphBuilder.AllocParameters<FMyShaderPS::FParameters>();
Parameters->MyFloat = 1.0f;
Parameters->MyVector = FVector3f(1, 0, 0);
Parameters->MyTexture = InputTexture;
```

### 셰이더 컴파일 시스템
```cpp
// 셰이더 컴파일 조건
static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
    // 특정 플랫폼에서만 컴파일
    return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
}

// 컴파일 환경 설정
static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, 
                                       FShaderCompilerEnvironment& OutEnvironment)
{
    FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
    
    // 셰이더 매크로 정의
    OutEnvironment.SetDefine(TEXT("TOON_SHADING_ENABLED"), 1);
    OutEnvironment.SetDefine(TEXT("MAX_TOON_STEPS"), 8);
}
```

---

## 🎮 에디터 확장 시스템

### Slate UI 시스템
```cpp
// Slate는 UE의 UI 프레임워크
// 선언적 문법 사용

class SMyWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyWidget) {}
        SLATE_ARGUMENT(float, InitialValue)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        ChildSlot
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Title", "NPR Settings"))
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SSlider)
                .Value(InArgs._InitialValue)
                .OnValueChanged(this, &SMyWidget::OnSliderChanged)
            ]
        ];
    }

private:
    void OnSliderChanged(float NewValue)
    {
        // 슬라이더 값 변경 처리
    }
};
```

### 에디터 메뉴 확장
```cpp
// 메뉴 확장 방법
void FToonForgeEditorModule::RegisterMenus()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    
    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
    MenuExtender->AddMenuExtension(
        "WindowLayout",                    // 확장할 메뉴 위치
        EExtensionHook::After,            // 위치 (Before/After)
        nullptr,                          // 명령 리스트
        FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& Builder)
        {
            Builder.AddMenuEntry(
                LOCTEXT("OpenNPRPanel", "NPR Control Panel"),
                LOCTEXT("OpenNPRPanelTooltip", "Open NPR Control Panel"),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([]()
                {
                    // 패널 열기 코드
                }))
            );
        })
    );
    
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}
```

---

## 💾 에셋 시스템 이해

### 에셋 생성 및 관리
```cpp
// 에셋 생성
UObject* CreateAsset(UClass* AssetClass, const FString& PackageName)
{
    UPackage* Package = CreatePackage(*PackageName);
    UObject* Asset = NewObject<UObject>(Package, AssetClass, *FPackageName::GetLongPackageAssetName(PackageName), RF_Public | RF_Standalone);
    
    // 에셋 등록
    FAssetRegistryModule::AssetCreated(Asset);
    Package->MarkPackageDirty();
    
    return Asset;
}

// 에셋 저장
bool SaveAsset(UObject* Asset)
{
    UPackage* Package = Asset->GetOutermost();
    FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
    
    return UPackage::SavePackage(Package, Asset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName);
}
```

### 커스텀 에셋 타입 생성
```cpp
// 커스텀 에셋 클래스
UCLASS(BlueprintType)
class TOONFORGE_API UNPRPresetAsset : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPR")
    FString PresetName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPR")
    float ToonSteps = 4.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPR")
    FLinearColor OutlineColor = FLinearColor::Black;
};

// 에셋 팩토리 (에디터에서 생성 가능하게 함)
UCLASS()
class TOONFORGEEDITOR_API UNPRPresetAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UNPRPresetAssetFactory()
    {
        SupportedClass = UNPRPresetAsset::StaticClass();
        bCreateNew = true;
        bEditAfterNew = true;
    }

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
    {
        return NewObject<UNPRPresetAsset>(InParent, Class, Name, Flags);
    }
};
```

---

## 🔧 성능 최적화 핵심 개념

### GPU 메모리 관리
```cpp
// 렌더 타겟 풀링 패턴
class FRenderTargetPool
{
    struct FPooledRT
    {
        TRefCountPtr<IPooledRenderTarget> RT;
        float LastUsedTime;
        FIntPoint Size;
        EPixelFormat Format;
    };
    
    TArray<FPooledRT> Pool;
    
public:
    TRefCountPtr<IPooledRenderTarget> FindFreeElement(const FIntPoint& Size, EPixelFormat Format)
    {
        // 1. 동일한 크기/포맷 찾기
        for (auto& Entry : Pool)
        {
            if (Entry.Size == Size && Entry.Format == Format && Entry.RT.GetRefCount() == 1)
            {
                Entry.LastUsedTime = FPlatformTime::Seconds();
                return Entry.RT;
            }
        }
        
        // 2. 새로 생성
        FPooledRenderTargetDesc Desc = FPooledRenderTargetDesc::Create2DDesc(Size, Format, FClearValueBinding::None, TexCreate_None, TexCreate_RenderTargetable | TexCreate_ShaderResource, false);
        TRefCountPtr<IPooledRenderTarget> NewRT;
        GRenderTargetPool.FindFreeElement(FRHICommandListExecutor::GetImmediateCommandList(), Desc, NewRT, TEXT("NPREffect"));
        
        Pool.Add({NewRT, FPlatformTime::Seconds(), Size, Format});
        return NewRT;
    }
};
```

### 배치 처리 최적화
```cpp
// 드로우콜 최소화 패턴
class FNPRBatchRenderer
{
    struct FBatchEntry
    {
        FMeshBatch MeshBatch;
        FMaterialRenderProxy* MaterialProxy;
        float Distance;  // 정렬용
    };
    
    TArray<FBatchEntry> BatchEntries;
    
public:
    void AddMesh(const FMeshBatch& MeshBatch, FMaterialRenderProxy* MaterialProxy, float Distance)
    {
        BatchEntries.Add({MeshBatch, MaterialProxy, Distance});
    }
    
    void ExecuteBatch(FRHICommandListImmediate& RHICmdList)
    {
        // 1. 머티리얼별로 정렬 (상태 변경 최소화)
        BatchEntries.Sort([](const FBatchEntry& A, const FBatchEntry& B)
        {
            return A.MaterialProxy < B.MaterialProxy;
        });
        
        // 2. 배치 실행
        FMaterialRenderProxy* CurrentMaterial = nullptr;
        for (const auto& Entry : BatchEntries)
        {
            if (Entry.MaterialProxy != CurrentMaterial)
            {
                // 머티리얼 상태 변경
                CurrentMaterial = Entry.MaterialProxy;
                SetMaterialState(RHICmdList, CurrentMaterial);
            }
            
            // 메시 렌더링
            RenderMesh(RHICmdList, Entry.MeshBatch);
        }
        
        BatchEntries.Reset();
    }
};
```

---

## 🐛 디버깅 및 프로파일링

### 로깅 시스템
```cpp
// 로그 카테고리 정의
DECLARE_LOG_CATEGORY_EXTERN(LogToonForge, Log, All);
DEFINE_LOG_CATEGORY(LogToonForge);

// 로그 레벨별 사용법
UE_LOG(LogToonForge, Fatal, TEXT("치명적 오류: %s"), *ErrorMessage);
UE_LOG(LogToonForge, Error, TEXT("오류 발생: %d"), ErrorCode);
UE_LOG(LogToonForge, Warning, TEXT("경고: 성능 저하 가능"));
UE_LOG(LogToonForge, Log, TEXT("일반 정보: NPR 효과 활성화"));
UE_LOG(LogToonForge, Verbose, TEXT("상세 정보: 파라미터 값 = %f"), Value);
UE_LOG(LogToonForge, VeryVerbose, TEXT("매우 상세한 디버그 정보"));

// 조건부 로깅
UE_CLOG(bDebugMode, LogToonForge, Warning, TEXT("디버그 모드에서만 출력"));

// 콘솔에서 로그 레벨 조정
// log LogToonForge VeryVerbose
```

### GPU 프로파일링
```cpp
// GPU 타이밍 측정
class FGPUProfiler
{
    TMap<FString, FRenderQueryRHIRef> TimerQueries;
    TMap<FString, float> LastResults;
    
public:
    void BeginEvent(FRHICommandListImmediate& RHICmdList, const FString& EventName)
    {
        if (!TimerQueries.Contains(EventName))
        {
            TimerQueries.Add(EventName, RHICreateRenderQuery(RQT_AbsoluteTime));
        }
        
        RHICmdList.EndRenderQuery(TimerQueries[EventName]);
    }
    
    void EndEvent(FRHICommandListImmediate& RHICmdList, const FString& EventName)
    {
        if (TimerQueries.Contains(EventName))
        {
            RHICmdList.EndRenderQuery(TimerQueries[EventName]);
        }
    }
    
    float GetLastTime(const FString& EventName)
    {
        if (TimerQueries.Contains(EventName))
        {
            uint64 Result = 0;
            if (RHIGetRenderQueryResult(TimerQueries[EventName], Result, false))
            {
                float TimeMS = Result * 1e-6f;  // 나노초를 밀리초로
                LastResults.Add(EventName, TimeMS);
                return TimeMS;
            }
        }
        
        return LastResults.FindRef(EventName);
    }
};

// 사용법
SCOPED_GPU_STAT(RHICmdList, ToonShading);  // 언리얼 내장 프로파일러
// 또는
Profiler.BeginEvent(RHICmdList, TEXT("ToonShading"));
// ... 렌더링 코드 ...
Profiler.EndEvent(RHICmdList, TEXT("ToonShading"));
```

---

## 📚 추가 학습 리소스

### 필수 UE5 문서
- **[Plugin Development](https://docs.unrealengine.com/5.3/en-US/plugins-in-unreal-engine/)**
- **[Rendering Architecture](https://docs.unrealengine.com/5.3/en-US/rendering-and-graphics-in-unreal-engine/)**
- **[Shader Development](https://docs.unrealengine.com/5.3/en-US/shader-development-in-unreal-engine/)**
- **[Slate UI Framework](https://docs.unrealengine.com/5.3/en-US/slate-ui-framework-in-unreal-engine/)**

### 커뮤니티 리소스
- **[Unreal Engine Source Code](https://github.com/EpicGames/UnrealEngine)** - 최고의 학습 자료
- **[Real-Time Rendering Resources](https://www.realtimerendering.com/)** - 렌더링 이론
- **[Shader Toy](https://www.shadertoy.com/)** - 셰이더 실험 및 학습

이 개념들을 이해하면 ToonForge 개발이 훨씬 수월해집니다!
