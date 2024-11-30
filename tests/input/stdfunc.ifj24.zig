const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("ifj.string: ");
    var str1: []u8 = ifj.string("str1");
    var str2 = ifj.string("str2");
    const str3 = ifj.concat(str1, str2);
    ifj.write(str3);
    ifj.write("\n");

    ifj.write("ifj.chr: ");
    var a = ifj.chr(65);
    ifj.write(a);
    ifj.write("\n");
}