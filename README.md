# Sephiria DX11

A portfolio-oriented source snapshot of a custom 2D game project built with C++ and DirectX 11.

C++와 DirectX 11로 직접 구현한 2D 게임 프로젝트의 이력서·포트폴리오용 소스 저장소입니다. 개인 프로젝트로 작성한 엔진 및 게임플레이 코드를 포함합니다.

## Highlights

- Custom rendering, scene, collision, animation, UI, and asset-management systems
- Component-based gameplay architecture
- HLSL shader sources for sprites, tile maps, meshes, UI, and post-processing
- Visual Studio project files for Windows

## Repository scope

This repository intentionally contains source code and shader source only. Build outputs, debug symbols, logs, local IDE state, third-party binaries, and game assets are excluded to keep the repository suitable for public review.

공개 검토에 필요한 C++ 및 HLSL 소스만 포함하며, 빌드 결과물·디버그 심볼·로그·IDE 사용자 설정·외부 바이너리·게임 에셋은 제외했습니다.

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
