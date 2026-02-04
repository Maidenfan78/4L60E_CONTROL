# QMD setup for this repo (optional)

Purpose: make it easy to *search* the project documentation (markdown/txt) with keyword + semantic + hybrid reranking.

This does **not** require changing your markdown structure.

## 1) Install QMD (Bun)

```bash
bun install -g https://github.com/tobi/qmd
```

## 2) Create a collection for this repo (only md/txt)

From anywhere:

```bash
qmd collection add "C:\\Users\\Sav\\ProgramingProjects\\4l60e_control" --name 4l60e --mask "**/*.{md,txt}"
```

## 3) Add context hints (helps ranking)

```bash
qmd context add qmd://4l60e "4L60-E manual-mode transmission controller project. Teensy 4.1 + Pi Zero 2W logger. Key docs: HARDWARE.md, FIRMWARE.md, CONTROLLER_PINOUT.md, EPC_SOLENOID_DRIVER_STAGE.md."
qmd context add qmd://4l60e/resources "Reference PDFs, wiring diagrams, connector pinouts, photos. Prefer text sidecars when available."
```

## 4) (Optional) Generate embeddings

```bash
qmd embed
```

## 5) Useful queries

```bash
qmd search "EPC"
qmd search "failsafe"
qmd query "what is the EPC duty/pressure convention and what are the hard limits"
qmd query "downshift protection rules"
qmd query "TCC enable conditions and unlock sequence"
```

## 6) If you want PDFs/images searchable

QMD is best with text. For the PDFs/images under `resources/`, generate **text sidecars** and keep them next to the source files (same basename, `.txt`).

This repo includes scripts under `tools/text-extract/` you can use if you install the required utilities.
