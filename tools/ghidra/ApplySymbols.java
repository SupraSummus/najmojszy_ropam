// Nałożenie naszych symboli (nazw i komentarzy) na program.
// Wywoływany jako -postScript z argumentem: ścieżka pliku symbols.txt.
//
// Format linii: ADRES RODZAJ NAZWA [KOMENTARZ]
//   RODZAJ: f = funkcja (tworzona, jeśli Ghidra jej nie wykryła),
//           d = etykieta danych.
// Linie puste i zaczynające się od '#' są pomijane.
// @category Ropam

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

import java.nio.file.Files;
import java.nio.file.Path;

public class ApplySymbols extends GhidraScript {

    @Override
    protected void run() throws Exception {
        int n = 0;
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            line = line.strip();
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }
            String[] p = line.split("\\s+", 4);
            Address a = toAddr(p[0]);
            String comment = p.length > 3 ? p[3] : null;
            if (p[1].equals("f")) {
                Function f = getFunctionAt(a);
                if (f == null) {
                    disassemble(a);
                    f = createFunction(a, p[2]);
                }
                if (f == null) {
                    printerr("nie udało się utworzyć funkcji " + p[0]);
                    continue;
                }
                f.setName(p[2], SourceType.USER_DEFINED);
                if (comment != null) {
                    f.setComment(comment);
                }
            } else {
                createLabel(a, p[2], true, SourceType.USER_DEFINED);
                if (comment != null) {
                    setPlateComment(a, comment);
                }
            }
            n++;
        }
        println("Nałożono symboli: " + n);
    }
}
