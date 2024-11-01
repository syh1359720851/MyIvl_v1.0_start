module sample3(clk, rst,o,p,q,a,b,c,d,e);
input clk,rst;
output o;
output p;
output q;
input  a;
input  b;
input  c;
input  d;
input  e;

wire  a;
wire  b;
wire  c;
wire  d;
wire  e;
wire  o;
wire  p;
wire  q;
wire  g;
wire  h;
wire  i;
wire  j;
wire  k;
wire  l;
wire  m;
wire  n;

assign g =  a  |  d ;
assign h =  a & c ;
assign i =  !c ;
assign j =  d  |  e ;
assign k =  g  |  h ;
assign l =  h & j ;
assign m =  i & j ;
assign n =  l & m ;
assign o =  b & k ;
assign p =  !g ;
assign q =  !n ;

endmodule
