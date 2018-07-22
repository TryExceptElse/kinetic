cimport cython as cy
cimport libcpp.memory as mem
from libcpp.string cimport string
from libcpp cimport bool
from libcpp.unordered_map cimport unordered_map

from server.model.orbit cimport Orbit, PyOrbit
from server.model.vector cimport Vector, PyVector
from server.model.system cimport System, PySystem


cdef extern from "body.h" namespace "kin" nogil:
    cppclass Body:
        Body(const string id, const double GM, const double r,
                Body *parent, Orbit *orbit)

        bool HasParent() const
        bool AddChild(mem.unique_ptr[Body] body)
        bool IsParent(const Body &body)
        Orbit Predict(const double t) const
        Vector PredictLocalPosition(const double t) const
        Vector PredictSystemPosition(const double t) const
        Vector PredictLocalVelocity(const double t) const
        Vector PredictSystemVelocity(const double t) const

        # getters

        const string& id() const
        const Body* parent() const
        const Orbit* orbit() const
        double mass() const
        double gm() const
        double radius() const
        # Calculates radius of sphere of influence. Returns -1 if no parent
        double sphere_of_influence() const
        const unordered_map[string, mem.unique_ptr[Body]]& children() const


@cy.final
cdef class PyBody:
    cdef Body* _body
    cdef bool owning
    cdef PySystem system  # Reference is kept to prevent out-of-order deletion.
    cdef str _str_id

    @staticmethod
    cdef inline PyBody wrap(Body* body):
        return PyBody(ptr=<long long>body)

    cdef inline Body* get(self):
        return self._body

    cpdef bool is_parent_of(self, PyBody other)
    cpdef PyVector predict_local_position(self, double t)
    cpdef PyVector predict_system_position(self, double t)
    cpdef PyVector predict_local_velocity(self, double t)
    cpdef PyVector predict_system_velocity(self, double t)
