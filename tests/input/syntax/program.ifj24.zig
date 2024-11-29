const ifj = @import("ifj24.zig");

pub fn main() void{
    var   b : ?i32 = ifj.readi32();
    const a : i32 = 5;
    if(b)|B|{
        _ = a + B;
        //ifj.write(ab);
    }
    else{
        b = 5.2;
    }
}
