# Changelog

Tutte le modifiche rilevanti a questo progetto saranno documentate in questo file.

Il formato segue, in modo semplificato, i principi di Keep a Changelog.
Le versioni seguono Semantic Versioning: MAJOR.MINOR.PATCH.

## [0.1.3] - 2026-05-01
### Changed
- Nuova logica di regolazione con pressione lunga/corta encoder:
  - pressione lunga da RUN -> regolazione ORARIO
  - pressioni brevi: HH -> MM -> SS -> salva/esce
  - seconda pressione lunga in set orario -> regolazione DATA
  - pressioni brevi: GG -> MM -> AA -> salva/esce
- Corretto doppio step encoder: ora ogni scatto varia di 1.

## [0.1.2] - 2026-04-26
### Added
- Aggiunto schema elettrico PDF in `hardware/schemi/orologio-datario-schema-elettrico-v01.pdf`.
- Aggiornati i file README con riferimenti alla documentazione hardware.
- Aggiunto riferimento allo schema PDF nella BOM (`docs/bom/bom.csv`).

## [0.1.1] - 2026-04-26
### Added
- Aggiunto file STL manopola encoder in `hardware/stl/orologio-manopola-v01.stl`.

## [0.1.0] - 2026-04-25
### Added
- Struttura iniziale progetto pronta per GitHub.
- Firmware Arduino importato in `firmware/orologio-datario-ds3231/`.
- Cartelle hardware e STL con convenzioni di naming.
- Template GitHub per Issue e Pull Request.
