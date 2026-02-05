param(
  [Parameter(Mandatory=$true)][string]$Root,
  [switch]$Force,
  [string]$Lang = "eng",
  [int]$MaxFileMb = 8
)

# OCR images under $Root into sidecar text files: <name>.<ext>.txt
# Requires: tesseract available on PATH.
# Notes:
# - This is intentionally conservative: skips very large images by default.
# - For wiring diagrams, OCR quality varies. Even rough text helps search.

if (-not (Test-Path $Root)) {
  throw "Root path not found: $Root"
}

$tesseract = Get-Command tesseract -ErrorAction SilentlyContinue
if (-not $tesseract) {
  throw "tesseract not found on PATH. Install Tesseract OCR and try again. (e.g., choco install tesseract)"
}

$maxBytes = $MaxFileMb * 1024 * 1024

$exts = @(".png", ".jpg", ".jpeg", ".tif", ".tiff", ".bmp", ".webp")

Get-ChildItem -LiteralPath $Root -Recurse -File | Where-Object {
  $exts -contains $_.Extension.ToLowerInvariant()
} | ForEach-Object {
  $img = $_.FullName
  if ($_.Length -gt $maxBytes) {
    return
  }

  $out = "$img.txt"
  if ((-not $Force) -and (Test-Path $out)) {
    return
  }

  # tesseract output is written to <outBase>.txt
  $outBase = "$img.ocr"

  try {
    & $tesseract.Source $img $outBase -l $Lang 2>$null | Out-Null

    $produced = "$outBase.txt"
    if (Test-Path $produced) {
      Move-Item -LiteralPath $produced -Destination $out -Force
    } else {
      Set-Content -LiteralPath $out -Value "[OCR] No output produced for: $img"
    }

    # Add a small header to make it obvious this is machine OCR.
    $header = "[OCR sidecar] source=$img lang=$Lang created=$(Get-Date -Format o)"
    $body = Get-Content -LiteralPath $out -Raw
    Set-Content -LiteralPath $out -Value ($header + "`r`n`r`n" + $body)

  } finally {
    # cleanup stray base files if any
    Remove-Item -LiteralPath "$outBase.txt" -ErrorAction SilentlyContinue
  }
}

Write-Host "Done. Generated/updated image OCR sidecar text files under: $Root"
