// Eksport zdekompilowanego C wszystkich funkcji do jednego pliku.
// Wywoływany jako -postScript z argumentem: ścieżka pliku wyjściowego.
// @category Ropam

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

import java.io.PrintWriter;

public class ExportDecomp extends GhidraScript {

    @Override
    protected void run() throws Exception {
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        int n = 0;
        try (PrintWriter out = new PrintWriter(getScriptArgs()[0], "UTF-8")) {
            for (Function f : currentProgram.getFunctionManager()
                    .getFunctions(true)) {
                DecompileResults r = dec.decompileFunction(f, 60, monitor);
                out.printf("// ==== %s @ %s%n", f.getName(),
                        f.getEntryPoint());
                if (r.decompileCompleted()) {
                    out.println(r.getDecompiledFunction().getC());
                } else {
                    out.println("// błąd: " + r.getErrorMessage());
                }
                n++;
            }
        }
        println("Wyeksportowano funkcji: " + n);
    }
}
