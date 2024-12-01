const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write("ifj.string: ");
    var str1: []u8 = ifj.string("str1");
    var str2 = ifj.string("str2");
    const str3 = ifj.concat(str1, str2);
    ifj.write(str3);
    ifj.write("\n");

    ifj.write("ifj.ord: ");
    var one_as_i32: i32 = ifj.ord(str1, 3);
    ifj.write(one_as_i32);
    ifj.write("\n");

    ifj.write("ifj.chr: ");
    var a = ifj.chr(65);
    ifj.write(a);
    ifj.write("\n");

    ifj.write("ifj.strcmp: ");
    var str4 = ifj.string("s");
    ifj.write(ifj.strcmp(str1, str1)); // 0
    ifj.write(ifj.strcmp(str1, str2)); // -1
    ifj.write(ifj.strcmp(str2, str1)); // 1
    ifj.write(ifj.strcmp(str1, str4)); // 1
    ifj.write(ifj.strcmp(str4, str1)); // -1
    ifj.write("\n");

    ifj.write("ifj.substring: ");
    ifj.write(ifj.substring(str1, 0, 3)); // str
    ifj.write(ifj.substring(str1, 1, 2)); // t
    ifj.write("\n");
}