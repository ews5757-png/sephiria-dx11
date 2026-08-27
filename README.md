# Sephiria DX11

A portfolio-oriented source snapshot of a custom 2D game project built with C++ and DirectX 11.

## Highlights

- Custom rendering, scene, collision, animation, UI, and asset-management systems
- Component-based gameplay architecture
- HLSL shader sources for sprites, tile maps, meshes, UI, and post-processing
- Visual Studio project files for Windows

## Repository scope

This repository intentionally contains source code and shader source only. Build outputs, debug symbols, logs, local IDE state, third-party binaries, and game assets are excluded to keep the repository suitable for public review.

Because external runtime libraries and original game assets are not distributed here, this snapshot is intended primarily for code review and portfolio presentation rather than as a ready-to-run game package.

## Structure

- `20251013da2/Include/` — engine and gameplay C++ source
- `Shaders/` — HLSL shader source
- `20251013da2.sln` — Visual Studio solution

## Requirements

- Windows
- Visual Studio 2022 with Desktop development with C++
- DirectX 11 SDK components available through the Windows SDK
- Project-specific third-party dependencies, including FMOD and DirectXTex

## License

No license is granted for reuse or redistribution. All rights reserved unless explicitly stated otherwise.

