# PlatformIO Release Guide for Ethernet3

This guide provides step-by-step instructions for releasing the Ethernet3 library to the PlatformIO registry, including automated GitHub Actions workflows.

## Prerequisites

1. **PlatformIO Account**: Register at [platformio.org](https://platformio.org)
2. **GitHub Repository**: Properly configured with library structure
3. **Library Manifest**: Valid `library.json` and `library.properties` files
4. **Testing**: Verified compatibility across target platforms

## Manual Release Process

### 1. Prepare Library Structure

Ensure your library follows PlatformIO standards:

```
Ethernet3/
├── library.json          # PlatformIO manifest
├── library.properties    # Arduino IDE manifest  
├── platformio.ini        # Platform configurations
├── src/                  # Library source code
├── examples/             # Example sketches
├── docs/                 # Documentation
├── test/                 # Unit tests
└── README.md            # Main documentation
```

### 2. Create library.json

```json
{
  "name": "Ethernet3",
  "version": "1.6.0",
  "description": "Modernized Ethernet library with W5100/W5500 support, multi-instance architecture, and platform optimizations",
  "keywords": [
    "ethernet",
    "w5100", 
    "w5500",
    "networking",
    "multicast",
    "multi-instance",
    "spi",
    "embedded"
  ],
  "repository": {
    "type": "git",
    "url": "https://github.com/Mapy542/Ethernet3.git"
  },
  "authors": [
    {
      "name": "Ethernet3 Contributors",
      "maintainer": true
    }
  ],
  "license": "LGPL-2.1",
  "homepage": "https://github.com/Mapy542/Ethernet3",
  "frameworks": ["arduino"],
  "platforms": ["atmelavr", "espressif32", "espressif8266", "ststm32", "raspberrypi", "teensy"],
  "dependencies": [],
  "export": {
    "exclude": [
      ".git*",
      ".vscode",
      "test",
      "*.tmp"
    ]
  },
  "examples": [
    {
      "name": "Basic Usage",
      "base": "examples/WebClient"
    },
    {
      "name": "Multi-Instance",
      "base": "examples/MultiInstanceDemo"
    },
    {
      "name": "UDP Multicast",
      "base": "examples/Ethernet3ModernizationDemo"
    }
  ]
}
```

### 3. Manual Registry Submission

1. **Login to PlatformIO**: 
   ```bash
   pio account login
   ```

2. **Package Library**:
   ```bash
   pio package pack --output ./dist/
   ```

3. **Publish to Registry**:
   ```bash
   pio package publish ./dist/Ethernet3-1.6.0.tar.gz
   ```

## Automated Release with GitHub Actions

### 1. Create Release Workflow

Create `.github/workflows/platformio-release.yml`:

```yaml
name: PlatformIO Library Release

on:
  release:
    types: [published]
  workflow_dispatch:
    inputs:
      version:
        description: 'Version to release'
        required: true
        default: '1.6.0'

jobs:
  validate:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        platform: [arduino_uno, esp32dev, nucleo_f401re]
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    
    - name: Install PlatformIO
      run: |
        pip install platformio
        pio upgrade --dev
        pio pkg install --global "tool-unity"
    
    - name: Build Examples
      run: |
        pio ci examples/WebClient --board=${{ matrix.platform }}
        pio ci examples/MultiInstanceDemo --board=${{ matrix.platform }}
        pio ci examples/Ethernet3ModernizationDemo --board=${{ matrix.platform }}
    
    - name: Run Tests
      run: |
        pio test -e native
  
  release:
    needs: validate
    runs-on: ubuntu-latest
    if: github.event_name == 'release'
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python  
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    
    - name: Install PlatformIO
      run: |
        pip install platformio
        pio upgrade --dev
    
    - name: Update Version
      run: |
        VERSION=${GITHUB_REF#refs/tags/v}
        sed -i "s/\"version\": \".*\"/\"version\": \"$VERSION\"/" library.json
        sed -i "s/version=.*/version=$VERSION/" library.properties
    
    - name: Login to PlatformIO
      run: |
        echo "${{ secrets.PLATFORMIO_AUTH_TOKEN }}" | pio account login --username "${{ secrets.PLATFORMIO_USERNAME }}" --password -
    
    - name: Package Library
      run: |
        mkdir -p dist
        pio package pack --output ./dist/
    
    - name: Publish to Registry
      run: |
        pio package publish ./dist/*.tar.gz
    
    - name: Upload Release Assets
      uses: actions/upload-release-asset@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        upload_url: ${{ github.event.release.upload_url }}
        asset_path: ./dist/Ethernet3-${{ github.event.release.tag_name }}.tar.gz
        asset_name: Ethernet3-${{ github.event.release.tag_name }}.tar.gz
        asset_content_type: application/gzip
```

### 2. Setup Secrets

In your GitHub repository settings, add these secrets:

- `PLATFORMIO_USERNAME`: Your PlatformIO username
- `PLATFORMIO_AUTH_TOKEN`: Your PlatformIO authentication token

Get your auth token:
```bash
pio account token
```

### 3. Create Pre-release Validation

Create `.github/workflows/platformio-ci.yml`:

```yaml
name: PlatformIO CI

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        platform: [arduino_uno, arduino_mega, esp32dev, esp8266, nucleo_f401re, bluepill]
        example: [WebClient, MultiInstanceDemo, Ethernet3ModernizationDemo]
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    
    - name: Install PlatformIO
      run: |
        pip install platformio
        pio upgrade --dev
    
    - name: Build Example
      run: |
        pio ci examples/${{ matrix.example }} --board=${{ matrix.platform }}

  library-check:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    
    - name: Install PlatformIO
      run: |
        pip install platformio
        pio upgrade --dev
    
    - name: Library Check
      run: |
        pio pkg install --library .
        pio lib check --library .
```

## Release Checklist

### Before Release

- [ ] Update version in `library.json` and `library.properties`
- [ ] Update `CHANGELOG.md` with new features/fixes
- [ ] Test examples on multiple platforms
- [ ] Update documentation
- [ ] Run CI/CD pipeline successfully
- [ ] Create git tag: `git tag v1.6.0 && git push origin v1.6.0`

### Release Process

1. **Create GitHub Release**:
   - Go to GitHub repository → Releases → Create new release
   - Tag: `v1.6.0`
   - Title: `Ethernet3 v1.6.0 - Multi-Instance Architecture`
   - Description: Release notes from CHANGELOG.md

2. **Automated Publishing**:
   - GitHub Action will automatically publish to PlatformIO registry
   - Monitor workflow execution for any errors

3. **Verification**:
   - Check library appears in PlatformIO registry
   - Test installation: `pio lib install "Ethernet3@^1.6.0"`
   - Verify examples work with fresh installation

### Post-Release

- [ ] Update main branch documentation
- [ ] Announce release in community channels
- [ ] Monitor for user feedback and issues
- [ ] Plan next version features

## Troubleshooting

### Common Issues

**Authentication Failed**:
```bash
# Regenerate token
pio account token --regenerate
```

**Package Validation Errors**:
```bash
# Check package structure
pio package pack --output ./dist/ --dry-run
```

**Build Failures**:
```bash
# Test locally first
pio ci examples/WebClient --board=arduino_uno --verbose
```

### Support

- PlatformIO Community: https://community.platformio.org/
- GitHub Issues: https://github.com/Mapy542/Ethernet3/issues
- Documentation: https://docs.platformio.org/