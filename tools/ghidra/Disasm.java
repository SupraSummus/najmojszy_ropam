// Wypisanie asemblera funkcji zaczynających się pod podanymi adresami.
// Wywoływany jako -postScript z argumentami: adresy (np. 0x9d052c94).
// @category Ropam

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class Disasm extends GhidraScript {

    @Override
    protected void run() throws Exception {
        for (String arg : getScriptArgs()) {
            Address a = toAddr(arg);
            Function f = getFunctionAt(a);
            if (f == null) {
                println("brak funkcji pod " + arg);
                continue;
            }
            println("==== " + f.getName() + " @ " + a);
            for (Instruction i : currentProgram.getListing()
                    .getInstructions(f.getBody(), true)) {
                println(i.getAddress() + "  " + i);
            }
        }
    }
}
