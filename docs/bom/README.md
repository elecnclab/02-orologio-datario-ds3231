# BOM (Elenco Componenti)

In questa cartella trovi l'elenco componenti del progetto in formato CSV.

## File
- `bom.csv`: distinta base principale.

## Come usarlo
- Compila le colonne `fornitore`, `sku` e `link` quando scegli i componenti finali.
- Mantieni una riga per componente.
- Se una voce cambia versione/modello, aggiorna `valore_modello` e aggiungi nota in `note`.

## Colonne usate
- `id`: progressivo riga
- `categoria`: es. elettronica, meccanica
- `componente`: nome breve
- `descrizione`: funzione della parte
- `quantita`: quantità necessaria
- `valore_modello`: valore o codice modello
- `formato_package`: formato fisico/file (es. Modulo, PCB, STL)
- `fornitore`: nome shop/distributore
- `sku`: codice articolo del fornitore
- `link`: URL acquisto o riferimento
- `note`: info extra
