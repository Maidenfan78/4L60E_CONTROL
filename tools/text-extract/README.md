# Text extraction helpers (optional)

These scripts are optional utilities to make `resources/` searchable (QMD or otherwise) by generating `.txt` sidecar files next to PDFs/images.

## PDF -> text (recommended)

Install Poppler (brings `pdftotext`). On Windows you can:
- `choco install poppler` (if you use Chocolatey)
- or download Poppler binaries and add to PATH

Then run:

```powershell
./pdf-to-text.ps1 -Root "C:\Users\Sav\ProgramingProjects\4l60e_control\resources"
```

This creates `*.pdf.txt` files next to each PDF.

## Image -> text (OCR)

If you want OCR for diagrams/pinouts, install Tesseract:
- `choco install tesseract`

Then a similar script can be added.

## Notes

- Keep sidecars small and close to source file.
- Don’t overwrite manually curated notes.
