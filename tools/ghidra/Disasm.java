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
            Function f = getFunctionContaining(a);
            if (f == null) {
                println("==== poza funkcją: " + arg);
                Instruction i = getInstructionAt(a);
                for (int n = 0; i != null && n < 60; n++) {
                    println(i.getAddress() + "  " + i);
                    i = i.getNext();
                }
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
