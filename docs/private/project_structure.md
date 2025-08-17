# ToonForge 프로젝트 구조 가이드

## 📁 전체 폴더 구조

```
ToonForge/
├── 📄 ToonForge.uplugin          # 플러그인 메인 설정 파일
├── 📄 .gitignore                 # Git 무시 파일 목록
├── 📄 README.md                  # GitHub 메인 문서
│
├── 📁 Source/                    # C++ 소스 코드
│   ├── 📁 ToonForge/            # 런타임 모듈
│   │   ├── 📁 Public/           # 공개 헤더 파일
│   │   ├── 📁 Private/          # 내부 구현 파일
│   │   └── 📄 ToonForge.Build.cs # 빌드 설정
│   │
│   ├── 📁 ToonForgeEditor/      # 에디터 모듈
│   │   ├── 📁 Public/           # 공개 헤더 파일
│   │   ├── 📁 Private/          # 내부 구현 파일
│   │   └── 📄 ToonForgeEditor.Build.cs # 빌드 설정
│   │
│   └── 📁 ToonForgeShaders/     # 셰이더 관련 C++ 코드
│
├── 📁 Shaders/                  # HLSL 셰이더 파일
│   ├── 📁 Private/              # 내부 셰이더
│   └── 📁 Public/               # 공개 셰이더 (include용)
│
├── 📁 Content/                  # 언리얼 에셋 파일
│   └── 📁 ToonForge/
│       ├── 📁 Materials/        # 머티리얼 에셋
│       ├── 📁 Textures/         # 텍스처 에셋
│       ├── 📁 Meshes/           # 메시 에셋
│       └── 📁 Presets/          # NPR 프리셋 파일
│
├── 📁 Config/                   # 설정 파일
├── 📁 Resources/                # 리소스 파일
│   ├── 📁 Icons/                # 플러그인 아이콘
│   └── 📁 Images/               # 문서용 이미지
│
├── 📁 docs/                     # 문서 파일
│   ├── 📁 public/               # GitHub 공개 문서
│   └── 📁 private/              # 개인 개발 문서
│
├── 📁 Binaries/                 # 컴파일된 바이너리 (자동 생성)
└── 📁 Intermediate/             # 중간 빌드 파일 (자동 생성)
```

---

## 🔧 주요 파일 설명

### 플러그인 설정 파일
- **`ToonForge.uplugin`**: 플러그인 메타데이터 및 모듈 정의
- **`*.Build.cs`**: 각 모듈의 빌드 설정 및 의존성

### 핵심 소스 파일 (생성 예정)
```
Source/ToonForge/Public/
├── ToonForge.h                  # 메인 모듈 헤더
├── ToonForgeManager.h           # NPR 시스템 매니저
├── NPRRenderingPipeline.h       # 렌더링 파이프라인
├── NPRPerformanceMonitor.h      # 성능 모니터링
├── Effects/
│   ├── INPREffect.h             # NPR 효과 인터페이스
│   ├── ToonShadingEffect.h      # 툰 셰이딩 효과
│   ├── OutlineEffect.h          # 아웃라인 효과
│   └── WatercolorEffect.h       # 수채화 효과
└── Shaders/
    ├── ToonShadingShaders.h     # 툰 셰이딩 셰이더 클래스
    └── OutlineShaders.h         # 아웃라인 셰이더 클래스

Source/ToonForgeEditor/Public/
├── ToonForgeEditor.h            # 에디터 모듈 헤더
├── NPRControlPanelWidget.h      # NPR 제어 패널 UI
├── NPRPresetManager.h           # 프리셋 관리자
└── Customizations/
    └── NPRSettingsCustomization.h # 디테일 패널 커스터마이징
```

### 셰이더 파일 (생성 예정)
```
Shaders/Private/
├── ToonShading.usf              # 툰 셰이딩 셰이더 ✅ 생성됨
├── OutlineDetection.usf         # 아웃라인 검출 셰이더
├── WatercolorEffect.usf         # 수채화 효과 셰이더
└── Common/
    ├── NPRCommon.ush            # NPR 공통 함수
    └── NPRStructures.ush        # NPR 구조체 정의
```

---

## 🚀 개발 시작 가이드

### 1. UE5 프로젝트에 플러그인 추가
```bash
# 1. UE5 프로젝트 생성 (C++ 프로젝트)
# 2. 프로젝트 폴더에 Plugins 디렉터리 생성
mkdir YourProject/Plugins

# 3. ToonForge 폴더를 Plugins에 복사
cp -r ToonForge YourProject/Plugins/

# 4. 프로젝트 파일 재생성
# 우클릭 .uproject → Generate Visual Studio project files
```

### 2. 빌드 및 테스트
```bash
# Visual Studio에서 빌드
# 또는 명령줄에서:
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" YourProject Win64 Development -Project="YourProject.uproject"
```

### 3. 에디터에서 플러그인 활성화
1. UE5 에디터 실행
2. `Edit → Plugins` 메뉴
3. "ToonForge" 검색 후 활성화
4. 에디터 재시작

### 4. NPR 제어 패널 열기
- `Window → ToonForge → NPR Control Panel`

---

## 📝 개발 워크플로우

### Day 1-2: 기본 구조 완성
1. **누락된 헤더 파일들 생성**
   - `ToonForgeManager.h/.cpp`
   - `NPRRenderingPipeline.h/.cpp`
   - `NPRPerformanceMonitor.h/.cpp`

2. **기본 컴파일 확인**
   - 플러그인이 UE5에서 로드되는지 확인
   - 로그 출력으로 모듈 초기화 확인

### Day 3-4: 첫 번째 NPR 효과
1. **툰 셰이딩 효과 클래스 구현**
   - `ToonShadingEffect.h/.cpp`
   - `ToonShadingShaders.h/.cpp`

2. **포스트 프로세싱 연결**
   - 기존 `ToonShading.usf` 셰이더 활용
   - C++ 클래스와 셰이더 연결

### Day 5-7: UI 및 제어 시스템
1. **NPR 제어 패널 구현**
   - `NPRControlPanelWidget.h/.cpp`
   - Slate UI로 파라미터 조정 인터페이스

2. **실시간 파라미터 업데이트**
   - Game Thread ↔ Render Thread 통신
   - 키보드 입력 처리

---

## 🔍 디버깅 및 개발 도구

### Visual Studio 설정
```json
// .vscode/c_cpp_properties.json (VS Code 사용 시)
{
    "configurations": [
        {
            "name": "UE5",
            "includePath": [
                "${workspaceFolder}/Source/**",
                "C:/Program Files/Epic Games/UE_5.3/Engine/Source/**"
            ],
            "defines": [
                "WITH_EDITOR=1",
                "UE_BUILD_DEVELOPMENT=1",
                "TOONFORGE_API="
            ],
            "compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
    "version": 4
}
```

### 유용한 UE5 콘솔 명령어
```
// 로그 레벨 설정
log LogToonForge VeryVerbose

// 셰이더 리컴파일
recompileshaders changed

// 성능 통계 표시
stat fps
stat gpu
stat scenerendering

// 렌더링 디버그
showflag.PostProcessing 0/1
r.ScreenPercentage 50
```

---

## 📦 배포 준비

### 플러그인 패키징
1. **에디터에서 패키징**
   - `Edit → Plugins → Package Plugin`
   - 대상 플랫폼 선택

2. **수동 패키징**
   ```bash
   # UnrealBuildTool을 사용한 패키징
   RunUAT.bat BuildPlugin -Plugin="ToonForge.uplugin" -Package="PackagedPlugin" -Rocket
   ```

### 배포 체크리스트
- [ ] 모든 플랫폼에서 컴파일 확인
- [ ] 샘플 프로젝트 테스트
- [ ] 문서 업데이트
- [ ] 라이선스 파일 포함
- [ ] 버전 번호 업데이트

---

이 구조를 기반으로 **체계적이고 확장 가능한 ToonForge 플러그인**을 개발할 수 있습니다!
