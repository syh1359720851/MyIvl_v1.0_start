module sample1(clk, rst,h,a,b,c,d);
input clk,rst;
output h;
input  a;
input  b;
input  c;
input  d;

wire  a;
wire  b;
wire  c;
wire  d;
wire  h;
wire  e;
wire  f;
wire  g;

assign e =  a  |  b ;
assign f =  !c ;
assign g =  f & d ;
assign h =  e  |  g ;

endmodule
