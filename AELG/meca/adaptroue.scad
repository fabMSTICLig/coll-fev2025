difference()
{
    cylinder(r=13/2,h=10, $fn=6);
    cylinder(r=6/2,h=10);
    translate([0,0,2/2])cube([5,15,2], center=true);
    translate([0,0,2/2])cube([15,5,2], center=true);
}