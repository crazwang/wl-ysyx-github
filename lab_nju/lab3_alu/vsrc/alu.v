module alu(a,b,sel,c,cf,of,out,zero);
    input [3:0] a;
    input [3:0] b;
    input [2:0] sel;
    output reg [3:0] c;
    output reg cf,of,out,zero;

    reg [3:0]b_bar;
    reg cf_bar;

    always @(a or b or sel) begin
        case(sel)
            3'b000 : begin
                        {cf,c} = a + b;
                        of = (a[3] == b[3])&& (c[3] != a[3]);
                        out = 1'b0;
                        zero = ~ (| c);
                        b_bar = 4'd0000;
                        cf_bar = 1'd0;
                     end
            3'b001 : begin
                        b_bar = ~b +1'b1;
                        {cf_bar,c} = a + b_bar;
                        cf = !cf_bar;
                        of = (a[3] == b_bar[3])&& (c[3] != a[3]);
                        out = 1'b0;
                        zero = ~(| c);              
                     end
            3'b010 : begin
                        c = ~ a;
                        {b_bar,cf_bar,cf,of,out,zero}={4'b0000,1'b0,1'b0,1'b0,1'b0,1'b0};
                     end
            3'b011 : begin
                        c = a & b;
                        {b_bar,cf_bar,cf,of,out,zero}={4'b0000,1'b0,1'b0,1'b0,1'b0,1'b0};
                     end
            3'b100 : begin
                        c = a | b;
                        {b_bar,cf_bar,cf,of,out,zero}={4'b0000,1'b0,1'b0,1'b0,1'b0,1'b0};
                     end                     
            3'b101 : begin
                        c = a ^ b;
                        {b_bar,cf_bar,cf,of,out,zero}={4'b0000,1'b0,1'b0,1'b0,1'b0,1'b0};
                     end
            3'b110 : begin
                        if(a[3] == 1 && b[3] == 0)
                            out = 1'b1 ;
                        else if(a[3] == 0 && b[3] ==1)
                            out = 1'b0 ;
                        else if(a[3] ==1 && b[3] ==1)
                            out = a>b ? 1'b1:1'b0;
                        else
                            out = a<b ? 1'b1:1'b0;
                        {b_bar,c,cf_bar,cf,of,zero}={4'b0000,4'b0000,1'b0,1'b0,1'b0,1'b0};
                     end
            3'b111 : begin
                        out = (a == b)? 1'b1:1'b0;
                        {b_bar,c,cf_bar,cf,of,zero}={4'b0000,4'b0000,1'b0,1'b0,1'b0,1'b0};
                     end
            default :   {b_bar,c,cf_bar,cf,of,out,zero}={4'b0000,4'b0000,1'b0,1'b0,1'b0,1'b0,1'b0};
        endcase   
    end
endmodule