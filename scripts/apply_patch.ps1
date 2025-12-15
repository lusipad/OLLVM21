[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$UpstreamRoot
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$upstream = Resolve-Path $UpstreamRoot

$srcPasses = Join-Path $repoRoot "llvm-project\\llvm\\lib\\Passes"
$dstPasses = Join-Path $upstream "llvm\\lib\\Passes"

if (-not (Test-Path $srcPasses)) {
    throw "Source Passes dir not found: $srcPasses"
}
if (-not (Test-Path $dstPasses)) {
    throw "Upstream Passes dir not found: $dstPasses (is this the root of llvm-project?)"
}

Copy-Item -Recurse -Force (Join-Path $srcPasses "*") $dstPasses

$srcTests = Join-Path $repoRoot "llvm-project\\llvm\\test\\Transforms\\Obfuscation"
$dstTests = Join-Path $upstream "llvm\\test\\Transforms\\Obfuscation"

if (Test-Path $srcTests) {
    New-Item -ItemType Directory -Force -Path $dstTests | Out-Null
    Copy-Item -Recurse -Force (Join-Path $srcTests "*") $dstTests
}

Write-Host "Applied OLLVM21 patch to: $upstream"

