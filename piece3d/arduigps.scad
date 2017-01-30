l = 110;
lg = 60;
h = 22;
e = 2;
$fn = 20;

arduino_mega_e = 1;
arduino_mega_l = 102;
arduino_mega_lg = 54;

arduino_uno_l = 68;
arduino_uno_lg = 54;

e_logo = 0.4;

m3_r = 1.9;

module m3(h){
    cylinder(h, r=m3_r);
}

arduino_drill = [[15,3]
    , [15,arduino_mega_lg-3]
    , [66, 7]
    , [66, 36]
    , [91, arduino_mega_lg-3]
    , [97, 3]];
    
module arduino_uno(){
    union(){
            difference(){
                cube([arduino_uno_l, arduino_uno_lg, arduino_mega_e]);
                for(i=[0:5]){
                    translate([0,0, -1]) translate(arduino_drill[i]) m3(10);
                }
            }
            translate([-8, 32, e]) cube([16, 12, 11]);
            translate([-5, 4, e]) cube([14, 9, 11]);
        }
}

screen_l = 80;
screen_lg = 58;

module screen(){
    difference(){
        union(){
            cube([screen_l, screen_lg, arduino_mega_e]);
            translate([4, 20.5, e]) cube([76, 25, 11]);
            translate([0, 15, 0]) cube([80, 36, 6]);
    
            translate([3.5, 52, e]) cube([20, 6, 11]);
            translate([6.5, 21 + 27.5, e]) cube([41, 1.5, 11]);
            translate([0, 1, 0])cube([46, 14, 11]);
        }
        for(i=[1,2]){
            translate([0, 2, -1]) translate([11, 0,0])translate(arduino_drill[i]) m3(10);
        }
    }
}

gps_lg =  26;
gps_l =  36;
module gy_gpsv3_neo(){
    translate([-gps_lg/2, -gps_l/2,0]) cube([gps_lg, gps_l, e]);
}

x_arduino = -arduino_mega_l/2 + 9;
y_arduino = -arduino_mega_lg/2;
 
gps_x =  gps_lg/2 +27;
gps_y =  0;

module boite(){
    union(){
        difference(){
            union(){
                translate([-l/2-e,-lg/2-e,0]) cube([l+2*e, lg+2*e, h+e]);
                hull(){
                    translate([0,(lg/2+3)]) cylinder(h+e, r = 2+e);
                    translate([0,-(lg/2+3)]) cylinder(h+e, r = 2+e);
                }
            }
            translate([-l/2,-lg/2,e]) cube([l, lg, h+e]);
            for(i=[0:3]){
                translate([x_arduino,y_arduino,-5]) translate(arduino_drill[i]) m3(10);
            }
            
            translate([-l/2, y_arduino+38, e+4+5.5]) cube([10, 13, 12], center = true);
            translate([-l/2, y_arduino+8.5, e+10.5]) rotate([0,90,0])cylinder(10,r=10/2, center=true);
            translate([l/2, -lg/2+15, h/2]) rotate([0,90,0])cylinder(10,r=7.5/2, center=true);
            
            translate([0,(lg/2+3), -1]) m3(h+e+2);
            translate([0,-(lg/2+3), -1]) m3(h+e+2);
            
            translate([25,-5, -1]) mirror([1,0,0])logo();
        }
        for(i=[0:3]){
            translate([x_arduino,y_arduino,0]) translate(arduino_drill[i])
            difference(){
                cylinder(2.5+e, r = 4);
                translate([0,0, -1]) m3(e+4);
            }
        }
        
        
        
    }
}

module logo(){
    font = "Liberation Sans:style=Bold";
    linear_extrude(height = e_logo+1) {
       text(text = "ArduiGps", font = font, size = 8);
     }
}

module boite_up(){
    d_e= 0.4;
    difference(){
        union(){
            translate([-l/2-2*e-d_e,-lg/2-2*e-d_e,0]) cube([l+4*e+2*d_e, lg+4*e+2*d_e, 2*e]);
            hull(){
                translate([0,(lg/2+3)]) cylinder(2*e, r = 2+2*e+2*d_e);
                translate([0,-(lg/2+3)]) cylinder(2*e, r = 2+2*e+2*d_e);
            }
        }
        union(){
            translate([-l/2-e-d_e,-lg/2-e-d_e,e]) cube([l+2*e+2*d_e, lg+2*e+2*d_e, e+1]);
            hull(){
                translate([0,(lg/2+3),e]) cylinder(e+1, r = 2+e+2*d_e);
                translate([0,-(lg/2+3),e]) cylinder(e+1, r = 2+e+2*d_e);
            }
            
        }
        mirror([1,0,0])translate([0,(-lg/4 - 10), -1])logo();
        mirror([1,0,0])translate([0,(lg/2+3), -1]) m3(h+2);
        mirror([1,0,0])translate([0,-(lg/2+3), -1]) m3(h+2);
        mirror([1,0,0])translate([x_arduino - 12,y_arduino - 3,3+11-h]) screen();
    }
}

i = 0;
if(i==0){
    boite();
    translate([0,0,h+e+1])rotate([0,180,0])boite_up();
    %color("red") translate([gps_x,gps_y,4]) gy_gpsv3_neo();
    %color("blue") translate([x_arduino,y_arduino,4]) arduino_uno();
    %color("red") translate([x_arduino - 12,y_arduino - 2,4+11]) screen();
} else if (i==1){
    boite();
} else {
    boite_up();

}