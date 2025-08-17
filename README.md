# 🎨 ToonForge
**Real-time NPR (Non-Photorealistic Rendering) Toolkit for Unreal Engine 5**

[![UE5](https://img.shields.io/badge/Unreal%20Engine-5.3+-blue.svg)](https://www.unrealengine.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Mac-lightgrey.svg)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

ToonForge is a comprehensive NPR rendering plugin for Unreal Engine 5 that provides real-time stylized rendering effects with artist-friendly tools and performance optimization.

## ✨ Features

### 🎭 NPR Effects
- **Toon Shading**: Customizable color quantization with 3-8 steps
- **Outline Rendering**: Multiple implementation methods (Edge Detection & Geometry Expansion)
- **Watercolor Effects**: Noise-based color variation with paper texture overlay
- **Real-time Control**: Instant parameter adjustment with keyboard shortcuts

### 🛠️ Artist Tools
- **Custom Editor Panel**: Dedicated NPR control interface
- **Preset System**: Save, load, and share NPR styles
- **Batch Processing**: Apply NPR effects to multiple objects at once
- **Interactive Demo**: Real-time showcase of all features

### ⚡ Performance Optimization
- **Real-time Monitoring**: GPU timing, memory usage, and draw call analysis
- **Automatic Quality Adjustment**: Maintain target FPS (30/60/120)
- **Memory Pooling**: Efficient render target reuse
- **LOD Integration**: Distance-based quality scaling

## 🎬 Demo

![ToonForge Demo](docs/images/demo_showcase.gif)

*Interactive demo showcasing toon shading, outline rendering, and watercolor effects*

## 🚀 Quick Start

### Prerequisites
- Unreal Engine 5.3 or later
- Visual Studio 2022 (for C++ development)
- Windows 10/11 or macOS 12+

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/ToonForge.git
   cd ToonForge
   ```

2. **Copy plugin to your project**
   ```bash
   cp -r ToonForge/Plugins/ToonForge YourProject/Plugins/
   ```

3. **Enable the plugin**
   - Open your UE5 project
   - Go to Edit → Plugins
   - Search for "ToonForge" and enable it
   - Restart the editor

4. **Open ToonForge panel**
   - Window → ToonForge → NPR Control Panel

### Basic Usage

1. **Apply Toon Shading**
   ```cpp
   // In Blueprint or C++
   AToonForgeManager* Manager = AToonForgeManager::Get();
   Manager->EnableToonShading(true);
   Manager->SetToonSteps(4);
   ```

2. **Keyboard Controls** (in Play mode)
   - `1-8`: Adjust toon shading steps
   - `Q/W`: Increase/decrease outline thickness
   - `A/S`: Toggle toon shading on/off
   - `Z/X`: Toggle outline rendering
   - `Tab`: Show/hide performance UI

## 🏗️ Architecture

### Core Components

```
ToonForge Plugin
├── FToonForgeManager          // Main plugin manager
├── FNPRRenderingPipeline      // Custom rendering pipeline
├── FNPREffectManager          // Effect management system
├── FNPRPerformanceMonitor     // Real-time performance tracking
└── UToonForgeEditorWidget     // Editor UI panel
```

### NPR Effects System

```cpp
// Extensible effect interface
class TOONFORGE_API INPREffect
{
public:
    virtual void Execute(FRHICommandListImmediate& RHICmdList, 
                        const FViewInfo& View) = 0;
    virtual bool IsEnabled() const = 0;
    virtual float GetRenderCost() const = 0;
};

// Built-in effects
class FToonShadingEffect : public INPREffect { /* ... */ };
class FOutlineEffect : public INPREffect { /* ... */ };
class FWatercolorEffect : public INPREffect { /* ... */ };
```

## 📊 Performance Analysis

### Benchmark Results
*Tested on RTX 3070, 1920x1080, Epic settings*

| Effect | GPU Time (ms) | Memory (MB) | FPS Impact |
|--------|---------------|-------------|------------|
| Toon Shading | 0.8ms | 12MB | ~5% |
| Outline (Edge Detection) | 1.2ms | 8MB | ~7% |
| Outline (Geometry) | 0.6ms | 4MB | ~3% |
| Watercolor | 1.5ms | 16MB | ~9% |
| **All Combined** | **3.1ms** | **32MB** | **~18%** |

### Optimization Features
- **Automatic LOD**: Quality scales with distance and performance
- **Render Target Pooling**: 60% reduction in memory allocations
- **Batch Processing**: Up to 40% fewer draw calls for similar objects
- **Shader Optimization**: Platform-specific optimizations

## 🎨 NPR Effects Gallery

<table>
<tr>
<td><img src="docs/images/toon_shading.jpg" width="200"/><br/><b>Toon Shading</b></td>
<td><img src="docs/images/outline_rendering.jpg" width="200"/><br/><b>Outline Rendering</b></td>
<td><img src="docs/images/watercolor.jpg" width="200"/><br/><b>Watercolor Effect</b></td>
</tr>
</table>

## 🛠️ Technical Highlights

### Custom Rendering Pipeline
- **Post-Processing Integration**: Seamless integration with UE5's post-process chain
- **RHI Utilization**: Direct use of Render Hardware Interface for optimal performance
- **Multi-threaded Rendering**: Proper RHI thread usage for async processing

### Shader Development
```hlsl
// Example: Toon Shading Pixel Shader
float4 ToonShadingPS(FScreenVertexOutput Input) : SV_Target0
{
    float3 BaseColor = SceneColorTexture.Sample(SceneColorSampler, Input.UV).rgb;
    
    // Color quantization
    float3 QuantizedColor = floor(BaseColor * ToonSteps) / ToonSteps;
    
    // Smooth transitions
    float3 FinalColor = smoothstep(QuantizedColor - SmoothWidth, 
                                  QuantizedColor + SmoothWidth, BaseColor);
    
    return float4(FinalColor, 1.0f);
}
```

### Editor Integration
- **Custom Asset Types**: NPR preset assets with thumbnail generation
- **Real-time Preview**: Viewport integration for immediate feedback
- **Undo/Redo Support**: Full editor integration with history system

## 📚 Documentation

- [**Installation Guide**](docs/installation.md) - Detailed setup instructions
- [**User Manual**](docs/user_manual.md) - Complete feature documentation
- [**API Reference**](docs/api_reference.md) - C++ and Blueprint API
- [**Performance Guide**](docs/performance.md) - Optimization tips and benchmarks
- [**Shader Reference**](docs/shaders.md) - Custom shader documentation

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup
1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes and test thoroughly
4. Commit with clear messages: `git commit -m 'Add amazing feature'`
5. Push to your fork: `git push origin feature/amazing-feature`
6. Create a Pull Request

## 🐛 Known Issues

- **Mobile Performance**: Watercolor effect may impact performance on lower-end mobile devices
- **VR Compatibility**: Some effects need optimization for VR rendering
- **Memory Usage**: Large scenes may require render target pool size adjustment

See [Issues](https://github.com/yourusername/ToonForge/issues) for current bug reports and feature requests.

## 🗺️ Roadmap

### Version 1.1 (Planned)
- [ ] Mobile optimization improvements
- [ ] Additional NPR effects (Sketch, Oil Painting)
- [ ] VR/AR support
- [ ] Preset sharing platform

### Version 1.2 (Future)
- [ ] AI-assisted style transfer
- [ ] Real-time collaboration tools
- [ ] Cloud rendering support
- [ ] Marketplace integration

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Epic Games** for Unreal Engine 5 and excellent documentation
- **Real-Time Rendering Community** for NPR research and techniques
- **Open Source Contributors** who helped improve this project

## 📞 Contact

- **Developer**: [Your Name]
- **Email**: your.email@example.com
- **LinkedIn**: [Your LinkedIn Profile]
- **Portfolio**: [Your Portfolio Website]

---

<p align="center">
  <b>⭐ If you find ToonForge useful, please consider giving it a star! ⭐</b>
</p>

<p align="center">
  Made with ❤️ for the Unreal Engine community
</p>
