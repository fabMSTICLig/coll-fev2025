$fn=100;

difference()
{
    union()
    {
        cylinder(r=15, h=42);
        cylinder(r=25, h=2);
    }
    cylinder(r=5, h=42);
}