param(
  [Parameter(Mandatory=$true)][string]$Root,
  [switch]$Force
)

# Converts PDFs under $Root into sidecar text files: <name>.pdf.txt
# Requires: pdftotext (Poppler) available on PATH.

if (-not (Test-Path $Root)) {
  throw "Root path not found: $Root"
}

$pdftotext = Get-Command pdftotext -ErrorAction SilentlyContinue
if (-not $pdftotext) {
  throw "pdftotext not found on PATH. Install Poppler (pdftotext) and try again."
}

Get-ChildItem -LiteralPath $Root -Recurse -File -Filter *.pdf | ForEach-Object {
  $pdf = $_.FullName
  $out = "$pdf.txt"

  if ((-not $Force) -and (Test-Path $out)) {
    return
  }

  # -layout keeps columns where possible
  & $pdftotext.Source -layout $pdf $out

  # Strip obviously empty outputs (common with scanned PDFs)
  try {
    $len = (Get-Item -LiteralPath $out).Length
    if ($len -lt 20) {
      # keep the file but mark it so you know it probably needs OCR
      Add-Content -LiteralPath $out -Value "`n[NOTE] Output is tiny; this PDF may be scanned-image and require OCR."
    }
  } catch {}
}

Write-Host "Done. Generated/updated PDF sidecar text files under: $Root"
