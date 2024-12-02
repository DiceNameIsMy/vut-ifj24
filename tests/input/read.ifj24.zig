const ifj = @import("ifj");

pub fn main() void {
    
    // Read a string
    const str = ifj.readstr();
    if (str) |s| {
        ifj.write(s);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read string\n");
    }

    // Read an i32
    const i32_val = ifj.readi32();
    if (i32_val) |i| {
        ifj.write(i);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read i32\n");
    }

    // Read a f64
    const f64_val = ifj.readf64();
    if (f64_val) |f| {
        ifj.write(f);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read f64\n");
    }

    // Read a string
    const str2 = ifj.readstr();
    if (str2) |s| {
        ifj.write(s);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read string\n");
    }

    // Read an i32
    const i32_val2 = ifj.readi32();
    if (i32_val2) |i| {
        ifj.write(i);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read i32\n");
    }

    // Read a f64
    const f64_val2 = ifj.readf64();
    if (f64_val2) |f| {
        ifj.write(f);
        ifj.write("\n");
    } else {
        ifj.write("Failed to read f64\n");
    }  
}