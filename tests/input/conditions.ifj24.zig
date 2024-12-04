const ifj = @import("ifj24.zig");

pub fn main() void {
    var a: i32 = 1;
    if (a == 1) {
        ifj.write("if(a == 1)\n");
    }

    if (a == 2) {
        ifj.write("if(a == 2)\n");
    } else {
        ifj.write("if(a == 2) else\n");
    }

    if (a == 3) {
        ifj.write("if(a == 3)\n");
    } else if (a == 1) {
        ifj.write("if(a == 3) else if(a == 1)\n");
    } else {
        ifj.write("if(a == 3) else if(a == 1) else\n");
    }

    if (a == 4) {
        ifj.write("if(a == 4)\n");
    } else if (a == 5) {
        ifj.write("if(a == 4) else if(a == 5)\n");
    } else {
        ifj.write("if(a == 4) else if(a == 5) else\n");
    }

    var out = 0;
    var b: ?i32 = ifj.readi32();
    if (b) |val| {
        out = val;
        ifj.write("if(b)\n");
    } else {
        ifj.write("if(b) else\n");
    }

    if (1 == 0) {
        ifj.write("if(1 == 0)\n");
    } else if (b) |val| {
        out = val;
        ifj.write("if(1 == 0) else if(b)\n");
    } else {
        ifj.write("if(1 == 0) else if(b) else\n");
    }

    var c: ?i32 = null;
    if (c) |val| {
        out = val;
        ifj.write("if(c)\n");
    } else {
        ifj.write("if(c) else\n");
    }

    if (1 == 0) {
        ifj.write("if(1 == 0)\n");
    } else if (c) |val| {
        out = val;
        ifj.write("if(1 == 0) else if(c)\n");
    } else {
        ifj.write("if(1 == 0) else if(c) else\n");
    }

    var d = 0;
    while (d < 2) {
        ifj.write("while(d < 2)\n");
        d = d + 1;
    }

}