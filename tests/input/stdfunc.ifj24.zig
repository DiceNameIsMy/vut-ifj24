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
    ifj.write(substring(str1, 0, 3)); // str
    ifj.write(substring(str1, 1, 2)); // t
    ifj.write("\n");
}

pub fn strcmp(a: []u8, b: []u8) i32 {
    var lena = ifj.length(a);
    var lenb = ifj.length(b);
    var minLen = lena;
    if (lenb < lena) {
        minLen = lenb;
    }

    var i = 0;
    while (i < minLen) {
        var aChar = ifj.ord(a, i);
        var bChar = ifj.ord(b, i);
        if (aChar < bChar) {
            return 0 - 1;
        } else if (aChar > bChar) {
            return 1;
        }
        i = i + 1;
    }
    if (lena < lenb) {
        return 0 - 1;
    } else if (lena > lenb) {
        return 1;
    }
    return 0;
}

pub fn substring(s: []u8, i: i32, j: i32) ?[]u8 {
    var len = ifj.length(s);
    if (i < 0 ) {
        return null;
    }
    if (j < 0 ) {
        return null;
    }
    if (i > j) {
        return null;
    }
    if (i >= len) {
        return null;
    }
    if ( j >= len) {
        return null;
    }
    var ssub: []u8 = ifj.string("");
    var idx = i;
    while (idx < j) {
        var charStr = ifj.chr(ifj.ord(s, idx));
        ssub = ifj.concat(ssub, charStr);
        idx = idx + 1;
    }
    return ssub;
}