param(
    [string]$Configuration,
    [string]$VcpkgInstalledDir
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

try {
    if ($Configuration -notin @('Debug', 'Release')) {
        throw 'The first argument must be Debug or Release.'
    }
    if ([string]::IsNullOrWhiteSpace($VcpkgInstalledDir)) {
        throw 'The second argument must be the active vcpkg installed triplet directory.'
    }

    $root = $PSScriptRoot
    $clientBin = Join-Path $root 'Client\Bin'
    $sdkInc = Join-Path $root 'EngineSDK\Inc'
    $sdkLib = Join-Path $root 'EngineSDK\Lib'
    $sourceResources = Join-Path $root 'MC_DX11_RESOURCE'
    $targetResources = Join-Path $clientBin 'Resources'
    if (-not [IO.Path]::IsPathRooted($VcpkgInstalledDir)) {
        $VcpkgInstalledDir = Join-Path $root $VcpkgInstalledDir
    }
    $vcpkgRoot = [IO.Path]::GetFullPath($VcpkgInstalledDir)
    $runtimeFolder = if ($Configuration -eq 'Debug') { 'debug\bin' } else { 'bin' }
    $vcpkgBin = Join-Path $vcpkgRoot $runtimeFolder

    $engineDll = Join-Path $root 'Engine\Bin\Engine.dll'
    $engineLib = Join-Path $root 'Engine\Bin\Engine.lib'
    $fmodDll = Join-Path $root 'ThirdParty\fmod_2_03_12\lib\x64\fmod.dll'
    foreach ($file in @($engineDll, $engineLib, $fmodDll)) {
        if (-not (Test-Path -LiteralPath $file -PathType Leaf)) {
            throw "Required build output or runtime file is missing: $file"
        }
    }
    $headers = @(Get-ChildItem -LiteralPath (Join-Path $root 'Engine\Public') -File)
    $runtimeDlls = @(Get-ChildItem -LiteralPath $vcpkgBin -Filter '*.dll' -File)
    if ($headers.Count -eq 0 -or $runtimeDlls.Count -eq 0) {
        throw "Engine headers or $Configuration vcpkg runtime DLLs are missing ($vcpkgBin)."
    }
    $resourceFolders = @('Font', 'Shader', 'Sound', 'Texture')
    foreach ($folder in $resourceFolders) {
        if (-not (Test-Path -LiteralPath (Join-Path $sourceResources $folder) -PathType Container)) {
            throw "Required resource folder is missing: $folder"
        }
    }

    $resourcesLinked = $false
    $existingResources = Get-Item -LiteralPath $targetResources -Force -ErrorAction SilentlyContinue
    if ($null -ne $existingResources) {
        if ($existingResources.Attributes -band [IO.FileAttributes]::ReparsePoint) {
            $targets = @($existingResources.Target)
            if ($targets.Count -ne 1 -or [string]::IsNullOrWhiteSpace($targets[0])) {
                throw "Cannot identify the existing Resources link target: $targetResources"
            }
            $linkTarget = [string]$targets[0]
            if (-not [IO.Path]::IsPathRooted($linkTarget)) {
                $linkTarget = Join-Path $clientBin $linkTarget
            }
            $linkTarget = [IO.Path]::GetFullPath($linkTarget).TrimEnd('\')
            $expectedTarget = [IO.Path]::GetFullPath($sourceResources).TrimEnd('\')
            if (-not $linkTarget.Equals($expectedTarget, [StringComparison]::OrdinalIgnoreCase)) {
                throw "Resources links to '$linkTarget', not '$expectedTarget'. No link was changed."
            }
            $resourcesLinked = $true
        } elseif (-not $existingResources.PSIsContainer) {
            throw "Resources exists but is not a directory: $targetResources"
        } else {
            $nestedLink = Get-ChildItem -LiteralPath $targetResources -Recurse -Force |
                Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint } |
                Select-Object -First 1
            if ($null -ne $nestedLink) {
                throw "Resources contains a link; refusing to copy through it: $($nestedLink.FullName)"
            }
        }
    }

    foreach ($directory in @($clientBin, $sdkInc, $sdkLib)) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }
    $headers | ForEach-Object { Copy-Item -LiteralPath $_.FullName -Destination $sdkInc -Force }
    Copy-Item -LiteralPath $engineLib -Destination $sdkLib -Force
    foreach ($dll in @($engineDll, $fmodDll) + @($runtimeDlls.FullName)) {
        Copy-Item -LiteralPath $dll -Destination $clientBin -Force
    }

    if (-not $resourcesLinked) {
        foreach ($folder in $resourceFolders) {
            $sourceFolder = Join-Path $sourceResources $folder
            New-Item -ItemType Directory -Path (Join-Path $targetResources $folder) -Force | Out-Null
            foreach ($file in Get-ChildItem -LiteralPath $sourceFolder -File -Recurse -Force) {
                $relativePath = $file.FullName.Substring($sourceResources.Length + 1)
                if ($file.Name -ieq 'desktop.ini' -or $relativePath -match '(^|[\\/])\.git([\\/]|$)') {
                    continue
                }
                $destination = Join-Path $targetResources $relativePath
                New-Item -ItemType Directory -Path ([IO.Path]::GetDirectoryName($destination)) -Force | Out-Null
                Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
            }
        }
    }
    $resourceMode = if ($resourcesLinked) { 'existing resource link preserved' } else { 'resources copied' }
    Write-Host "Prepared Client/Bin for $Configuration ($resourceMode)."
    exit 0
} catch {
    [Console]::Error.WriteLine('CopyClient: ' + $_.Exception.Message)
    exit 1
}
