package objects

import info.laht.threekt.core.BufferGeometry
import info.laht.threekt.core.Object3D
import info.laht.threekt.geometries.SphereBufferGeometry
import info.laht.threekt.materials.Material
import info.laht.threekt.materials.MeshStandardMaterial
import info.laht.threekt.objects.Mesh

open class Spheroid(
        name: String="",
        id: String="",
        radius: Double=1.0,
        mass: Double=1.0
): Body(name, id, radius, mass) {

    override var threeObject: Object3D = makeMesh()

    private fun makeMesh(): Mesh {
        val geometry: BufferGeometry = SphereBufferGeometry(radius, 32, 16)
        val material: Material = MeshStandardMaterial()
        return Mesh(geometry, material)
    }

}