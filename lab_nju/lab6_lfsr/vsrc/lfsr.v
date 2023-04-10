module lfsr( data_in,seg_out,data_out,clk,set_initial,zero,count);
    input [7:0]data_in;
    input clk,set_initial;

    output [13:0]seg_out;
    output zero;
    output [20:0]count;
    output reg[7:0]data_out;

    reg[3:0]one;
    reg[3:0]hundred;
    reg[3:0]ten;
    reg up_ten,up_hundred;

    always@(clk or set_initial) begin
        if(set_initial) 
            data_out = data_in;
        else if(clk)
            data_out = {data_out[4]^data_out[3]^data_out[2]^data_out[0],data_out[7:1]};
        else
            data_out = data_out;
    end
    
    always @(clk or set_initial) begin
        if(set_initial) begin
            one = 4'b0000;
        end
        else if(clk) begin
            if(one ==4'd9) 
                one = 4'd0;
            else 
                one = one+1'b1;
        end
        else begin
            one =one;
        end
    end

    assign up_ten =(clk && one == 4'd9)?1'b1:1'b0;

    always@(clk or set_initial)begin
        if(set_initial)
            ten = 4'd0;
        else if(up_ten)begin
            if(ten == 4'd9) 
                ten = 4'd0;
            else
                ten = ten+1'b1;
        end
        else
            ten = ten;
    end

    assign up_hundred =(up_ten && one == 4'd9 && ten == 4'd9)?1'b1:1'b0;

    always@(clk or set_initial)begin
        if(set_initial)
            hundred = 4'd0;
        else if(up_hundred) begin
            if(hundred == 4'd9)
                hundred =4'd0;
            else
                hundred = hundred+1'b1;
        end
        else
            hundred=hundred;
    end

    assign zero = ~(|data_out);

    bcd7seg seg0(data_out[3:0],seg_out[6:0]);
    bcd7seg seg1(data_out[7:4],seg_out[13:7]);

    bcd7seg seg2(one,count[6:0]);
    bcd7seg seg3(ten,count[13:7]);
    bcd7seg seg4(hundred,count[20:14]);

endmodule
