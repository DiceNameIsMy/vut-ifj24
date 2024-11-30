const ifj = @import("ifj24.zig");

pub fn main() void {
    var str1: []u8 = ifj.string("str1");
    var str2 = ifj.string("str2");
    const str3 = ifj.concat(str1, str2);
    ifj.write(str3);
}