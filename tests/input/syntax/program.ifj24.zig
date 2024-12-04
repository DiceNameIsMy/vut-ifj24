const ifj = @import("ifj24.zig");

pub fn main() void {
    var a = 5;
    if (a < 5) {
        a = a + 1;
    }
    _ = a;
}
