package objects

import util.G

abstract class Body(
        name: String="",
        id: String="",
        val radius: Double=1.0,
        val mass: Double=1.0
): GameObject(name, id) {
    val mu = G * mass
    /** standard gravitational parameter */
}