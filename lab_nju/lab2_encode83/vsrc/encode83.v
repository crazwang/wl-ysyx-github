module encode83(x,o_led,o_seg,y);
    input [7:0] x;
    output reg [2:0] o_led;
    output [6:0] o_seg;
    output y;

    assign y = | x;
    always @(x) begin
        casez (x)
            8'b1??????? : o_led = 3'b111; 
            8'b01?????? : o_led = 3'b110; 
            8'b001????? : o_led = 3'b101; 
            8'b0001???? : o_led = 3'b100; 
            8'b00001??? : o_led = 3'b011; 
            8'b000001?? : o_led = 3'b010; 
            8'b0000001? : o_led = 3'b001; 
            8'b00000001 : o_led = 3'b000; 
            default : o_led = 3'b000;
        endcase
    end

    bcd7seg seg0(.b({1'b0,o_led}),.h(o_seg));

endmodule
