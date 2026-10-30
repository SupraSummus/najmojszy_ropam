// Przygotowanie obrazu firmware'u PIC32MX do analizy:
// bloki RAM i rejestrów peryferiów, wartość $gp, punkt wejścia.
// Wywoływany jako -preScript z argumentem: wartość $gp (np. 0xA0007FF0).
// @category Ropam

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

import java.math.BigInteger;

public class PrepPic32 extends GhidraScript {

    @Override
    protected void run() throws Exception {
        long gp = Long.decode(getScriptArgs()[0]);
        Memory mem = currentProgram.getMemory();

        // RAM 128 KB widoczny przez KSEG0 i KSEG1
        uninit(mem, "ram_kseg0", 0x80000000L, 0x20000);
        uninit(mem, "ram_kseg1", 0xA0000000L, 0x20000);
        // Rejestry peryferiów (SFR)
        MemoryBlock sfr = uninit(mem, "sfr", 0xBF800000L, 0x100000);
        sfr.setVolatile(true);

        Register gpReg = currentProgram.getRegister("gp");
        Address lo = addr(0x9D000000L);
        Address hi = addr(0x9D07FFFFL);
        currentProgram.getProgramContext()
                .setValue(gpReg, lo, hi, BigInteger.valueOf(gp));

        Address entry = addr(0x9D001000L);
        createLabel(entry, "_reset", true, SourceType.USER_DEFINED);
        addEntryPoint(entry);
        disassemble(entry);

        // Wektory przerwań: 64 wpisy co 0x20 od EBASE + 0x200
        for (int i = 0; i < 64; i++) {
            Address v = addr(0x9D000200L + 0x20L * i);
            createLabel(v, String.format("vector_%02d", i), true,
                    SourceType.USER_DEFINED);
            disassemble(v);
        }
        Address gen = addr(0x9D000180L);
        createLabel(gen, "_general_exception", true, SourceType.USER_DEFINED);
        disassemble(gen);
    }

    private Address addr(long a) {
        return currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(a);
    }

    private MemoryBlock uninit(Memory mem, String name, long start, long len)
            throws Exception {
        MemoryBlock b = mem.createUninitializedBlock(name, addr(start), len,
                false);
        b.setRead(true);
        b.setWrite(true);
        return b;
    }
}
