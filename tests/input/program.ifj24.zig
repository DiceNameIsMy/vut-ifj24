// Program 1: Vypocet faktorialu (iterativne)
const ifj = @import("ifj24.zig");

// Hlavni telo programu - funkce main
pub fn main() void {
    ifj.write("Zadejte cislo pro vypocet faktorialu\n");
    const val = 5;
    if (val == val) {
        if (val < 0) {
            ifj.write("Faktorial ");
            ifj.write(" nelze spocitat\n");
        } else {
            var d: f64 = ifj.i2f(val);
            var vysl: f64 = 1.0;
            while (d > 0) {
                vysl = vysl * d;
                d = d - 1.0;
            }
            const vysl_i32 = ifj.f2i(vysl);
        }
    } else { // a == null
        ifj.write("Faktorial pro null nelze spocitat\n");
    }
}