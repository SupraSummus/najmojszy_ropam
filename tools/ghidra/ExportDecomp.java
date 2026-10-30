// Eksport zdekompilowanego C.
// Wywoływany jako -postScript z argumentami:
//   1. ścieżka pliku na wszystkie funkcje,
//   2. (opcjonalnie) katalog na funkcje nazwane przez nas,
//      po jednym pliku NAZWA.c (stare pliki *.c są z niego usuwane).
// @category Ropam

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

import java.io.File;
import java.io.PrintWriter;

public class ExportDecomp extends GhidraScript {

    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        File namedDir = args.length > 1 ? new File(args[1]) : null;
        if (namedDir != null) {
            namedDir.mkdirs();
            for (File old : namedDir.listFiles((d, n) -> n.endsWith(".c"))) {
                old.delete();
            }
        }

        DecompileOptions opts = new DecompileOptions();
        opts.grabFromProgram(currentProgram);
        opts.setPLATECommentIncluded(true);
        DecompInterface dec = new DecompInterface();
        dec.setOptions(opts);
        dec.openProgram(currentProgram);

        int all = 0;
        int named = 0;
        try (PrintWriter out = new PrintWriter(args[0], "UTF-8")) {
            for (Function f : currentProgram.getFunctionManager()
                    .getFunctions(true)) {
                DecompileResults r = dec.decompileFunction(f, 60, monitor);
                String c = r.decompileCompleted()
                        ? r.getDecompiledFunction().getC()
                        : "// błąd: " + r.getErrorMessage() + "\n";
                String head = String.format("// ==== %s @ %s%n",
                        f.getName(), f.getEntryPoint());
                out.print(head);
                out.println(c);
                all++;
                if (namedDir != null
                        && f.getSymbol().getSource() == SourceType.USER_DEFINED) {
                    File file = new File(namedDir, f.getName() + ".c");
                    try (PrintWriter w = new PrintWriter(file, "UTF-8")) {
                        w.printf("// %s @ %s%n", f.getName(), f.getEntryPoint());
                        w.println("// Wygenerowane przez tools/ghidra/analyze.sh;"
                                + " nie edytować ręcznie.");
                        w.print(c);
                    }
                    named++;
                }
            }
        }
        println("Wyeksportowano funkcji: " + all + ", nazwanych: " + named);
    }
}
