# ToonForge Usage Guide

## 🚀 Quick Start

### System Requirements
- **Unreal Engine**: 5.3 or later
- **Platform**: Windows 10/11, macOS 12+
- **Graphics**: DirectX 11/12, Vulkan, or Metal support
- **Memory**: 8GB RAM minimum, 16GB recommended
- **Storage**: 500MB free space

### Installation

#### Method 1: Plugin Installation (Recommended)
1. **Download ToonForge**
   ```bash
   git clone https://github.com/yourusername/ToonForge.git
   ```

2. **Copy to your project**
   ```bash
   # Copy plugin folder to your project
   cp -r ToonForge/Plugins/ToonForge YourProject/Plugins/
   ```

3. **Enable in Unreal Editor**
   - Open your UE5 project
   - Go to `Edit → Plugins`
   - Search for "ToonForge"
   - Check the box to enable it
   - Restart the editor when prompted

4. **Verify installation**
   - Look for `Window → ToonForge` in the menu bar
   - You should see "NPR Control Panel" option

#### Method 2: Source Code Integration
1. **Add to project modules**
   ```json
   // YourProject.uproject
   {
     "Modules": [
       {
         "Name": "ToonForge",
         "Type": "Runtime"
       }
     ]
   }
   ```

2. **Generate project files**
   ```bash
   # Right-click .uproject file → Generate Visual Studio project files
   ```

3. **Build the project**
   ```bash
   # Build in Visual Studio or using UnrealBuildTool
   ```

---

## 🎨 First Steps

### Opening NPR Control Panel
1. **Access the panel**
   - Menu: `Window → ToonForge → NPR Control Panel`
   - Or use keyboard shortcut: `Ctrl + Shift + N`

2. **Panel overview**
   ```
   NPR Control Panel
   ├── 🎭 Toon Shading Section
   ├── 🖼️ Outline Section  
   ├── 🎨 Watercolor Section
   ├── ⚡ Performance Monitor
   └── 💾 Preset Management
   ```

### Your First NPR Effect

#### Step 1: Enable Toon Shading
1. In NPR Control Panel, find "Toon Shading" section
2. Check the "Enable" checkbox
3. You should immediately see the effect in the viewport

#### Step 2: Adjust Parameters
1. **Shading Steps**: Start with 4 (good balance)
2. **Shadow Intensity**: Try 0.3 for subtle shadows
3. **Shadow Color**: Keep default blue-gray or customize

#### Step 3: Add Outline
1. Check "Enable" in Outline section
2. Set thickness to 1.0 pixels
3. Keep color as black for classic look

#### Step 4: Save Your Settings
1. Click "Save Preset" button
2. Name it "My First Style"
3. Click "Save"

**🎉 Congratulations! You've created your first NPR style!**

---

## 🎮 Interactive Controls

### Keyboard Shortcuts (Play Mode)
```
=== NPR Effects Toggle ===
A / S    : Toon Shading On/Off
Z / X    : Outline On/Off
C / V    : Watercolor On/Off

=== Parameter Adjustment ===
1 - 8    : Toon Steps (1=smooth, 8=stepped)
Q / W    : Shadow Intensity -/+
E / R    : Outline Thickness -/+
F / G    : Watercolor Intensity -/+
H / J    : Noise Scale -/+

=== System Controls ===
Tab      : Show/Hide Performance UI
P        : Detailed Performance Info
T        : Switch Outline Method
O        : Auto Quality Adjustment On/Off
```

### Mouse Controls
- **Ctrl + Mouse Wheel**: Global NPR intensity
- **Right-click + Drag**: Real-time parameter adjustment mode
- **Middle-click**: Reset parameter to default

### Testing Your Effects
1. **Enter Play Mode** (`Alt + P`)
2. **Use keyboard shortcuts** to adjust effects in real-time
3. **Press Tab** to see performance impact
4. **Exit Play Mode** (`Escape`) when satisfied

---

## 🛠️ Detailed Configuration

### Toon Shading Settings

#### Basic Parameters
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Shading Steps** | 2-8 | 4 | Number of color quantization levels |
| **Shadow Intensity** | 0.0-1.0 | 0.3 | How dark the shadows appear |
| **Shadow Color** | RGB | (0.2, 0.2, 0.3) | Tint color for shadow areas |

#### Advanced Parameters
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Highlight Threshold** | 0.0-1.0 | 0.8 | When highlights start to appear |
| **Highlight Intensity** | 0.0-3.0 | 1.5 | Brightness of highlights |
| **Rim Power** | 0.5-5.0 | 2.0 | Sharpness of rim lighting |
| **Rim Intensity** | 0.0-2.0 | 0.5 | Strength of rim lighting |

#### Usage Tips
- **Animation Style**: 3-4 steps, soft shadows (0.2-0.4)
- **Comic Book Style**: 5-6 steps, strong shadows (0.5-0.7)
- **Game Style**: 4-5 steps, balanced settings

### Outline Settings

#### Method Selection
**Edge Detection Method**
- ✅ High quality outlines
- ✅ Works with complex geometry
- ❌ Higher GPU cost (1.2ms)
- 🎯 Best for: Hero objects, close-up views

**Geometry Expansion Method**
- ✅ Very fast performance (0.6ms)
- ✅ Consistent line thickness
- ❌ Limited to simple geometry
- 🎯 Best for: Background objects, distant views

#### Parameters
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Thickness** | 0.1-5.0 | 1.0 | Outline width in pixels |
| **Color** | RGB | (0, 0, 0) | Outline color |
| **Depth Threshold** | 0.01-1.0 | 0.1 | Depth edge sensitivity |
| **Normal Threshold** | 0.1-2.0 | 0.5 | Normal edge sensitivity |

#### Optimization Tips
```
Performance Priority:
High → Use Geometry Expansion
Medium → Auto-select based on distance
Low → Always use Edge Detection

Quality Priority:
High → Always use Edge Detection
Medium → Hybrid approach
Low → Geometry Expansion acceptable
```

### Watercolor Settings

#### Core Parameters
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Effect Intensity** | 0.0-1.0 | 0.5 | Overall watercolor strength |
| **Noise Scale** | 1.0-10.0 | 5.0 | Texture detail level |
| **Bleeding Radius** | 1-10 | 3 | Color bleeding distance |
| **Paper Intensity** | 0.0-1.0 | 0.3 | Paper texture strength |

#### Advanced Settings
- **Color Saturation**: Adjust color vibrancy (0.5-1.5)
- **Edge Softness**: Control edge blending (0.1-1.0)
- **Noise Layers**: Multiple noise scales (3 layers default)

---

## 💾 Preset Management

### Using Built-in Presets

#### Available Presets
1. **Animation Style**
   - Toon Steps: 3
   - Soft shadows and subtle outlines
   - Perfect for character animation

2. **Comic Book**
   - Toon Steps: 5
   - Strong black outlines
   - High contrast shadows

3. **Watercolor**
   - Toon Steps: 4
   - Watercolor effect enabled
   - Soft, artistic look

4. **Sketch**
   - Thin outlines only
   - Minimal toon shading
   - Clean, simple style

#### Loading Presets
1. **From NPR Control Panel**:
   - Click "Load Preset" dropdown
   - Select desired preset
   - Click "Apply"

2. **From Keyboard** (Play Mode):
   - `Ctrl + 1`: Animation Style
   - `Ctrl + 2`: Comic Book
   - `Ctrl + 3`: Watercolor
   - `Ctrl + 4`: Sketch

### Creating Custom Presets

#### Step-by-Step Process
1. **Adjust parameters** to your liking
2. **Test in different lighting** conditions
3. **Check performance** impact (Tab key)
4. **Save preset**:
   - Click "Save Preset"
   - Enter name and description
   - Optionally add tags
   - Click "Save"

#### Preset File Format
```json
{
  "name": "My Custom Style",
  "description": "Perfect for outdoor scenes",
  "author": "Your Name",
  "version": "1.0",
  "settings": {
    "toonShading": {
      "enabled": true,
      "steps": 4,
      "shadowIntensity": 0.4
    },
    "outline": {
      "enabled": true,
      "thickness": 1.2,
      "color": [0, 0, 0]
    }
  }
}
```

### Sharing Presets
1. **Export**: `NPR Control Panel → Export Preset`
2. **Share file**: Send `.npr` file to others
3. **Import**: `NPR Control Panel → Import Preset`

---

## ⚡ Performance Optimization

### Performance Monitoring

#### Real-time Performance UI
**Enable**: Press `Tab` in Play Mode
```
Performance Display:
┌─────────────────────────┐
│ FPS: 58.3               │
│ NPR GPU Time: 3.1ms     │
│ Memory Usage: 32MB      │
│                         │
│ Effect Breakdown:       │
│ • Toon Shading: 0.8ms   │
│ • Outline: 1.2ms        │
│ • Watercolor: 1.1ms     │
└─────────────────────────┘
```

#### Performance Targets
- **60 FPS**: NPR should use < 4ms GPU time
- **30 FPS**: NPR should use < 8ms GPU time
- **Memory**: Keep under 64MB for 4K resolution

### Auto Quality Adjustment

#### Setup
1. **Enable auto-optimization**: Press `O` in Play Mode
2. **Set target FPS**: 
   - 30 FPS: Conservative (mobile-friendly)
   - 60 FPS: Balanced (recommended)
   - 120 FPS: High-end (competitive gaming)

#### How It Works
```
Performance Monitoring Loop:
1. Measure current FPS
2. Compare with target FPS
3. If below target → Reduce NPR quality
4. If above target → Increase NPR quality
5. Repeat every 60 frames
```

#### Quality Levels
- **Ultra**: All effects at maximum quality
- **High**: Full effects with minor optimizations
- **Medium**: Reduced sampling, simplified effects
- **Low**: Essential effects only
- **Off**: NPR disabled

### Manual Optimization Tips

#### For Better Performance
1. **Use Geometry Expansion** outlines when possible
2. **Reduce watercolor intensity** (biggest performance impact)
3. **Lower toon steps** for distant objects
4. **Enable auto-optimization** for dynamic scenes

#### For Better Quality
1. **Use Edge Detection** outlines for hero objects
2. **Increase sampling** in watercolor effects
3. **Higher toon steps** for close-up views
4. **Disable auto-optimization** for cinematic scenes

---

## 🎯 Common Use Cases

### Game Development

#### Character Rendering
```
Recommended Settings:
- Toon Steps: 4-5
- Outline: 1.0-1.5px, black
- Method: Geometry Expansion (performance)
- Watercolor: Disabled (clarity)
```

#### Environment Rendering
```
Recommended Settings:
- Toon Steps: 3-4 (softer)
- Outline: 0.5-1.0px, dark gray
- Method: Edge Detection (quality)
- Watercolor: 0.2-0.4 (subtle)
```

### Animation/Film

#### Character Close-ups
```
Recommended Settings:
- Toon Steps: 3-4 (smooth)
- Outline: 1.5-2.0px, black
- Method: Edge Detection (quality)
- Watercolor: Disabled or very subtle
```

#### Stylized Environments
```
Recommended Settings:
- Toon Steps: 4-5
- Outline: 0.8-1.2px, colored
- Method: Hybrid (auto-select)
- Watercolor: 0.3-0.6 (artistic)
```

### Architectural Visualization

#### Clean Technical Style
```
Recommended Settings:
- Toon Steps: 5-6 (clear separation)
- Outline: 0.5-1.0px, dark gray
- Method: Edge Detection
- Watercolor: Disabled
```

---

## 🐛 Troubleshooting

### Common Issues

#### "NPR effects not visible"
**Possible Causes**:
- Effects not enabled in NPR Control Panel
- Global intensity set to 0
- Post-processing disabled in project settings

**Solutions**:
1. Check NPR Control Panel checkboxes
2. Verify global intensity > 0
3. Enable post-processing: `Project Settings → Rendering → Post Processing`

#### "Poor performance/low FPS"
**Possible Causes**:
- High resolution (4K+) with all effects enabled
- Complex scenes with many objects
- Outdated graphics drivers

**Solutions**:
1. Enable auto-optimization (`O` key)
2. Reduce watercolor intensity
3. Use Geometry Expansion outlines
4. Update graphics drivers

#### "Outlines appear jagged"
**Possible Causes**:
- Low anti-aliasing settings
- Geometry Expansion method on complex meshes
- Insufficient outline thickness

**Solutions**:
1. Increase anti-aliasing: `Project Settings → Rendering → Anti-Aliasing`
2. Switch to Edge Detection method
3. Increase outline thickness to 1.5+

#### "Watercolor effect looks noisy"
**Possible Causes**:
- Noise scale too high
- Effect intensity too strong
- Poor quality noise texture

**Solutions**:
1. Reduce noise scale (3.0-5.0 range)
2. Lower effect intensity (0.3-0.5)
3. Check noise texture quality settings

### Performance Issues

#### GPU Memory Warnings
```
Warning: NPR effects using excessive VRAM
Current usage: 128MB (Target: <64MB)

Solutions:
1. Reduce rendering resolution
2. Enable render target pooling
3. Disable unused effects
4. Lower texture quality
```

#### Frame Rate Drops
```
Warning: Inconsistent frame rate detected
Average FPS: 45 (Target: 60)

Solutions:
1. Enable auto-optimization
2. Profile individual effects (Tab key)
3. Reduce effect complexity
4. Check for other performance bottlenecks
```

### Getting Help

#### Debug Information
**Enable detailed logging**:
1. Console command: `log LogToonForge VeryVerbose`
2. Check `Saved/Logs/` folder for detailed logs
3. Include logs when reporting issues

#### Community Support
- **GitHub Issues**: Report bugs and feature requests
- **Discord**: Real-time community help
- **Documentation**: Check docs for detailed guides
- **Video Tutorials**: Step-by-step visual guides

---

## 📚 Next Steps

### Learning Resources
1. **Video Tutorials**: Watch the complete ToonForge series
2. **Example Projects**: Download sample scenes
3. **Advanced Techniques**: Custom shader development
4. **Community Showcase**: See what others have created

### Advanced Usage
- **Custom Shaders**: Extend ToonForge with your own effects
- **Blueprint Integration**: Control NPR effects via Blueprint
- **C++ API**: Full programmatic control
- **Batch Processing**: Automate NPR application to multiple assets

### Contributing
- **Report Issues**: Help improve ToonForge
- **Share Presets**: Contribute to the preset library
- **Documentation**: Help improve guides and tutorials
- **Code Contributions**: Submit pull requests

---

**🎉 You're now ready to create amazing NPR art with ToonForge!**

For more advanced topics, check out our [Technical Challenges](TECHNICAL_CHALLENGES.md) and [Architecture](ARCHITECTURE.md) documentation.
