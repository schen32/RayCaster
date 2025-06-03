# RayCaster
Implementing a ray caster that calculates any intersections inbetween the start and end of the ray. Ray casting is a useful technique in games, commonly used to simulate light and shadow, bullets, object trajectories, and more.

## Single Ray
![ezgif-7798da777d28cc](https://github.com/user-attachments/assets/c294f6e7-14bd-4936-91b3-17f388717be5)

Casting a single ray from the center of the screen to the position of the mouse. The ray stops if it intersects with any line segments, otherwise the ray continues to the mouse pointer.

## 100 Rays
![ezgif-4542abfc716855](https://github.com/user-attachments/assets/c5d0bcfb-a3b0-42f5-a77a-1f126174cb5f)

Now lets try emitting 100 rays outwards from the player position. We can see that the behaviour of the rays emulates the behaviour of light. Depending on the specific application in games, we can vary the number of rays or the angle of rays to create our desired effect. One such application could be the line of sight of enemies to the player.
