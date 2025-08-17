# ToonForge 트러블슈팅 가이드

## 🚨 컴파일 및 빌드 문제

### 문제: "Module 'ToonForge' could not be loaded"
**증상:**
- 에디터 시작 시 플러그인 로드 실패
- "The following modules are missing or built with a different engine version" 오류

**해결 방법:**
1. **프로젝트 파일 재생성**
   ```bash
   # .uproject 파일 우클릭 → Generate Visual Studio project files
   # 또는 명령줄에서:
   UnrealBuildTool.exe -projectfiles -project="YourProject.uproject" -game -rocket -progress
   ```

2. **빌드 설정 확인**
   ```cpp
   // ToonForge.Build.cs 파일 확인
   public class ToonForge : ModuleRules
   {
       public ToonForge(ReadOnlyTargetRules Target) : base(Target)
       {
           // 필수 의존성 확인
           PublicDependencyModuleNames.AddRange(new string[]
           {
               "Core",
               "CoreUObject",
               "Engine"
           });
       }
   }
   ```

3. **모듈 구현 확인**
   ```cpp
   // ToonForge.cpp 파일에서
   IMPLEMENT_MODULE(FToonForgeModule, ToonForge)  // 모듈 이름 일치 확인
   ```

### 문제: 셰이더 컴파일 실패
**증상:**
- "Failed to compile shader" 오류
- 검은 화면 또는 핑크색 머티리얼

**해결 방법:**
1. **셰이더 경로 확인**
   ```cpp
   // 올바른 셰이더 등록
   IMPLEMENT_GLOBAL_SHADER(FToonShadingPS, "/ToonForge/Private/ToonShading.usf", "MainPS", SF_Pixel);
   //                                       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 가상 경로 사용
   ```

2. **셰이더 파일 문법 확인**
   ```hlsl
   // ToonShading.usf - 기본 구조 확인
   #include "/Engine/Private/Common.ush"  // 절대 경로 사용
   
   float4 MainPS(FScreenVertexOutput Input) : SV_Target0  // 함수명 일치
   {
       return float4(1, 0, 0, 1);  // 간단한 테스트부터 시작
   }
   ```

3. **셰이더 캐시 정리**
   ```
   // 에디터 콘솔에서
   recompileshaders changed
   
   // 또는 프로젝트 폴더에서 캐시 삭제
   DerivedDataCache/ 폴더 삭제 후 재시작
   ```

### 문제: 링크 에러 (LNK2019, LNK2001)
**증상:**
- "unresolved external symbol" 오류
- 빌드는 되지만 링크 단계에서 실패

**해결 방법:**
1. **API 매크로 확인**
   ```cpp
   // 헤더 파일에서
   class TOONFORGE_API FToonForgeManager  // API 매크로 필수
   {
   public:
       TOONFORGE_API void Initialize();    // 공개 함수에 API 매크로
   };
   ```

2. **의존성 모듈 추가**
   ```cpp
   // Build.cs 파일에서 필요한 모듈 추가
   PrivateDependencyModuleNames.AddRange(new string[]
   {
       "RenderCore",  // 렌더링 관련 함수 사용 시
       "Renderer",    // 렌더링 파이프라인 접근 시
       "RHI"          // RHI 함수 사용 시
   });
   ```

3. **전방 선언 vs 포함**
   ```cpp
   // 헤더 파일에서는 전방 선언 사용
   class FRHICommandListImmediate;  // 전방 선언
   
   // 구현 파일에서 실제 헤더 포함
   #include "RHICommandList.h"      // 실제 구현에서 포함
   ```

---

## 🎨 렌더링 문제

### 문제: NPR 효과가 화면에 나타나지 않음
**증상:**
- 플러그인은 로드되지만 시각적 변화 없음
- 로그에 에러는 없음

**해결 방법:**
1. **포스트 프로세싱 활성화 확인**
   ```
   // 에디터 콘솔에서 확인
   showflag.PostProcessing 1
   
   // 프로젝트 설정 확인
   Project Settings → Rendering → Post Processing → Enable
   ```

2. **간단한 테스트 셰이더**
   ```hlsl
   // 화면을 빨간색으로 만드는 테스트
   float4 MainPS(FScreenVertexOutput Input) : SV_Target0
   {
       return float4(1, 0, 0, 1);  // 빨간색 출력
   }
   ```

3. **렌더링 파이프라인 연결 확인**
   ```cpp
   void FToonForgeModule::StartupModule()
   {
       UE_LOG(LogToonForge, Warning, TEXT("ToonForge Module Started"));  // 로그 확인
       
       // 렌더링 델리게이트에 연결
       GetRendererModule().RegisterPostOpaqueRenderDelegate(
           FPostOpaqueRenderDelegate::CreateRaw(this, &FToonForgeModule::RenderNPREffects)
       );
   }
   ```

### 문제: 성능이 매우 느림
**증상:**
- FPS가 크게 떨어짐
- GPU 사용률이 100%에 근접

**해결 방법:**
1. **GPU 프로파일링**
   ```
   // 에디터 콘솔에서
   stat gpu
   stat scenerendering
   
   // NPR 효과별 시간 측정
   SCOPED_GPU_STAT(RHICmdList, ToonShading);
   ```

2. **해상도 스케일링**
   ```cpp
   // NPR 효과를 낮은 해상도로 렌더링
   float NPRScale = 0.5f;  // 절반 해상도
   FIntPoint NPRSize = FIntPoint(
       ViewSize.X * NPRScale,
       ViewSize.Y * NPRScale
   );
   ```

3. **불필요한 계산 제거**
   ```hlsl
   // 비효율적인 코드
   float4 MainPS(FScreenVertexOutput Input) : SV_Target0
   {
       float3 Color = SceneTexture.Sample(Sampler, Input.UV).rgb;
       
       // 매 픽셀마다 복잡한 계산
       for(int i = 0; i < 100; i++)
       {
           Color = normalize(Color + sin(i * 0.1f));  // 불필요한 반복
       }
       
       return float4(Color, 1.0f);
   }
   
   // 최적화된 코드
   float4 MainPS(FScreenVertexOutput Input) : SV_Target0
   {
       float3 Color = SceneTexture.Sample(Sampler, Input.UV).rgb;
       
       // 간단한 색상 양자화
       Color = floor(Color * ToonSteps) / ToonSteps;
       
       return float4(Color, 1.0f);
   }
   ```

### 문제: 메모리 사용량 과다
**증상:**
- "Out of video memory" 오류
- 시스템이 느려짐

**해결 방법:**
1. **렌더 타겟 크기 최적화**
   ```cpp
   // 4K 해상도에서 메모리 사용량 (RGBA8 기준)
   // 3840 * 2160 * 4 bytes = 33MB per render target
   
   // 해상도 제한
   FIntPoint MaxNPRSize = FIntPoint(1920, 1080);  // Full HD로 제한
   FIntPoint ActualSize = FIntPoint(
       FMath::Min(ViewSize.X, MaxNPRSize.X),
       FMath::Min(ViewSize.Y, MaxNPRSize.Y)
   );
   ```

2. **렌더 타겟 포맷 최적화**
   ```cpp
   // 메모리 사용량 비교 (1920x1080 기준)
   PF_FloatRGBA    // 32MB (32bit * 4채널)
   PF_R16G16B16A16 // 16MB (16bit * 4채널)  
   PF_R8G8B8A8     // 8MB  (8bit * 4채널)   ← NPR에 적합
   
   // NPR용 최적화된 포맷
   EPixelFormat GetNPRFormat()
   {
       return PF_R8G8B8A8;  // 대부분의 NPR 효과에 충분
   }
   ```

3. **렌더 타겟 풀링**
   ```cpp
   // 사용 후 즉시 해제하지 말고 풀로 반환
   class FNPRRenderTargetPool
   {
       TArray<TRefCountPtr<IPooledRenderTarget>> Pool;
       
   public:
       void ReleaseToPool(TRefCountPtr<IPooledRenderTarget> RT)
       {
           if (Pool.Num() < 4)  // 최대 4개까지 풀링
           {
               Pool.Add(RT);
           }
           // 초과 시 자동 해제됨
       }
   };
   ```

---

## 🎮 에디터 통합 문제

### 문제: NPR 제어 패널이 열리지 않음
**증상:**
- 메뉴에서 클릭해도 패널이 나타나지 않음
- "Failed to spawn tab" 오류

**해결 방법:**
1. **탭 등록 확인**
   ```cpp
   void FToonForgeEditorModule::StartupModule()
   {
       // 탭 스포너 등록
       FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
           NPRControlPanelTabId,
           FOnSpawnTab::CreateRaw(this, &FToonForgeEditorModule::CreateNPRControlPanel)
       )
       .SetDisplayName(LOCTEXT("NPRControlPanelTitle", "NPR Control Panel"))
       .SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsCategory());
   }
   ```

2. **위젯 생성 확인**
   ```cpp
   TSharedRef<SDockTab> FToonForgeEditorModule::CreateNPRControlPanel(const FSpawnTabArgs& Args)
   {
       return SNew(SDockTab)
           .TabRole(ETabRole::NomadTab)
           [
               SNew(SNPRControlPanelWidget)  // 위젯 클래스 존재 확인
           ];
   }
   ```

3. **모듈 로딩 순서 확인**
   ```json
   // .uplugin 파일에서
   {
       "Name": "ToonForgeEditor",
       "Type": "Editor",
       "LoadingPhase": "PostEngineInit"  // 에디터 초기화 후 로드
   }
   ```

### 문제: UI 파라미터 변경이 반영되지 않음
**증상:**
- 슬라이더를 움직여도 NPR 효과에 변화 없음
- 로그에 파라미터 변경 메시지 없음

**해결 방법:**
1. **콜백 함수 연결 확인**
   ```cpp
   // Slate 위젯에서
   SNew(SSlider)
   .Value(this, &SNPRControlPanel::GetToonSteps)
   .OnValueChanged(this, &SNPRControlPanel::OnToonStepsChanged)  // 콜백 연결 확인
   
   void SNPRControlPanel::OnToonStepsChanged(float NewValue)
   {
       UE_LOG(LogToonForge, Log, TEXT("Toon Steps Changed: %f"), NewValue);  // 로그 확인
       
       // Game Thread에서 Render Thread로 전달
       ENQUEUE_RENDER_COMMAND(UpdateToonSteps)(
           [NewValue](FRHICommandListImmediate& RHICmdList)
           {
               GetGlobalToonSteps() = NewValue;
           });
   }
   ```

2. **스레드 안전성 확인**
   ```cpp
   // ❌ 잘못된 방법 - 직접 접근
   void OnToonStepsChanged(float NewValue)
   {
       GlobalToonSteps = NewValue;  // 스레드 안전하지 않음!
   }
   
   // ✅ 올바른 방법 - 렌더 명령 사용
   void OnToonStepsChanged(float NewValue)
   {
       ENQUEUE_RENDER_COMMAND(UpdateToonSteps)(
           [NewValue](FRHICommandListImmediate& RHICmdList)
           {
               GlobalToonSteps = NewValue;  // Render Thread에서 안전하게 업데이트
           });
   }
   ```

---

## 🔧 성능 및 최적화 문제

### 문제: 프레임 드롭 및 불안정한 성능
**증상:**
- FPS가 불규칙하게 변동
- 특정 상황에서만 성능 저하

**해결 방법:**
1. **성능 모니터링 추가**
   ```cpp
   class FNPRPerformanceMonitor
   {
       TArray<float> FrameTimes;
       float AverageFrameTime = 0.0f;
       
   public:
       void RecordFrameTime(float DeltaTime)
       {
           FrameTimes.Add(DeltaTime);
           if (FrameTimes.Num() > 60)  // 1초간 평균
           {
               FrameTimes.RemoveAt(0);
           }
           
           AverageFrameTime = 0.0f;
           for (float Time : FrameTimes)
           {
               AverageFrameTime += Time;
           }
           AverageFrameTime /= FrameTimes.Num();
           
           // 성능 저하 감지
           if (AverageFrameTime > 0.033f)  // 30 FPS 이하
           {
               UE_LOG(LogToonForge, Warning, TEXT("Performance degradation detected: %f ms"), AverageFrameTime * 1000.0f);
           }
       }
   };
   ```

2. **자동 품질 조정**
   ```cpp
   void FNPRQualityManager::AdjustQuality()
   {
       float CurrentFPS = 1.0f / AverageFrameTime;
       float TargetFPS = 60.0f;
       
       if (CurrentFPS < TargetFPS * 0.9f)  // 10% 여유
       {
           // 품질 낮추기
           if (CurrentQualityLevel > ENPRQuality::Low)
           {
               CurrentQualityLevel = (ENPRQuality)((int32)CurrentQualityLevel - 1);
               ApplyQualitySettings(CurrentQualityLevel);
           }
       }
       else if (CurrentFPS > TargetFPS * 1.1f)  // 10% 여유
       {
           // 품질 높이기
           if (CurrentQualityLevel < ENPRQuality::Ultra)
           {
               CurrentQualityLevel = (ENPRQuality)((int32)CurrentQualityLevel + 1);
               ApplyQualitySettings(CurrentQualityLevel);
           }
       }
   }
   ```

### 문제: 메모리 누수
**증상:**
- 시간이 지날수록 메모리 사용량 증가
- 에디터가 점점 느려짐

**해결 방법:**
1. **스마트 포인터 사용**
   ```cpp
   // ❌ 원시 포인터 사용
   class FToonForgeManager
   {
       FNPREffect* ToonEffect;  // 메모리 누수 위험
       
   public:
       ~FToonForgeManager()
       {
           delete ToonEffect;  // 수동 해제 필요
       }
   };
   
   // ✅ 스마트 포인터 사용
   class FToonForgeManager
   {
       TUniquePtr<FNPREffect> ToonEffect;  // 자동 해제
       
   public:
       // 소멸자에서 자동으로 해제됨
   };
   ```

2. **렌더 리소스 생명주기 관리**
   ```cpp
   class FNPRRenderResource : public FRenderResource
   {
   public:
       virtual void InitRHI() override
       {
           // GPU 리소스 생성
           TextureRHI = RHICreateTexture2D(...);
       }
       
       virtual void ReleaseRHI() override
       {
           // GPU 리소스 해제 (필수!)
           TextureRHI.SafeRelease();
       }
       
   private:
       FTexture2DRHIRef TextureRHI;
   };
   ```

3. **델리게이트 정리**
   ```cpp
   void FToonForgeModule::ShutdownModule()
   {
       // 등록한 델리게이트 정리
       if (GetRendererModule().IsValidLowLevel())
       {
           GetRendererModule().RemovePostOpaqueRenderDelegate(RenderDelegate);
       }
       
       // 타이머 정리
       if (GEngine && GEngine->GetWorld())
       {
           GEngine->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
       }
   }
   ```

---

## 🔍 디버깅 도구 및 팁

### 유용한 콘솔 명령어
```
// 렌더링 관련
stat fps                    // FPS 표시
stat gpu                    // GPU 통계
stat scenerendering         // 씬 렌더링 통계
r.ScreenPercentage 50       // 렌더링 해상도 50%로 설정
showflag.PostProcessing 0   // 포스트 프로세싱 끄기

// 셰이더 관련
recompileshaders changed    // 변경된 셰이더 재컴파일
recompileshaders all        // 모든 셰이더 재컴파일
r.ShaderDevelopmentMode 1   // 셰이더 개발 모드

// 로깅 관련
log LogToonForge VeryVerbose  // 상세 로그 활성화
log list                      // 모든 로그 카테고리 표시
```

### Visual Studio 디버깅 설정
```cpp
// 조건부 브레이크포인트 설정
void FToonForgeManager::UpdateEffect()
{
    if (EffectIntensity > 0.5f)  // 특정 조건에서만 중단
    {
        int BreakHere = 0;  // 브레이크포인트 설정
    }
}

// 디버그 출력
#if UE_BUILD_DEBUG
    UE_LOG(LogToonForge, Log, TEXT("Debug: Effect Intensity = %f"), EffectIntensity);
#endif
```

### RenderDoc 사용법
1. **RenderDoc 설치 및 설정**
2. **UE5 에디터를 RenderDoc으로 실행**
3. **F12 키로 프레임 캡처**
4. **NPR 효과의 GPU 사용량 분석**

이 트러블슈팅 가이드를 통해 개발 중 마주칠 수 있는 대부분의 문제들을 해결할 수 있을 것입니다!
