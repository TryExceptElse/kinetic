"""
Script prototyping orbit prediction
"""
import os
import sys

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# Add server root to path so that imports work properly regardless
# where script is called from.
root_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if root_path not in sys.path:
    sys.path.append(root_path)

import server.model.const as const
import server.model.path as path
from server.model.system import PySystem
from server.model.body import PyBody
from server.model.vector import PyVector

N_POINTS = 1000


exposed_functions = {}


def expose(f):
    exposed_functions[f.__name__] = f
    return f


@expose
def position_plot() -> None:
    body = PyBody(gm=const.G * 1.98891691172467e30, r=10)
    system = PySystem(root=body)
    r = PyVector(617_244_712_358.0, -431_694_791_368.0, -12_036_457_087.0)
    v = PyVector(7320.0, 11329.0, -0211.0)
    path_ = path.PyFlightPath(system, r, v, 0)

    period0 = 374942509.78053558
    performance = path.PyPerformanceData(3000, 20000)  # ve, thrust
    burn_start_t = period0 / 2

    maneuver = path.PyManeuver(
        path.PyManeuver.Types.Prograde,
        2000,
        performance,
        150,
        burn_start_t
    )
    path_.add(maneuver)

    # Get points to plot
    x_pts = []
    y_pts = []

    for i in range(N_POINTS):
        t = period0 / N_POINTS * i
        kinematic_data: path.PyKinematicData = path_.predict(t)
        x_pts.append(kinematic_data.r.x)
        y_pts.append(kinematic_data.r.y)

    burn_start_prediction = path_.predict(burn_start_t)
    burn_start_pos = burn_start_prediction.r

    # Plot

    plt.figure(1)
    plt.subplots_adjust(hspace=0.5, wspace=0.35)
    plt.plot(0, 0, 'ro')  # Body position.
    plt.plot(x_pts, y_pts, 'r-')  # line
    plt.plot(burn_start_pos.x, burn_start_pos.y, 'ro')  # current pos
    plt.axis('equal')
    plt.show()


@expose
def burn_plot() -> None:
    body = PyBody(gm=const.G * 1.98891691172467e30, r=10)
    system = PySystem(root=body)
    r = PyVector(617_244_712_358.0, -431_694_791_368.0, -12_036_457_087.0)
    v = PyVector(7320.0, 11329.0, -0211.0)
    path_ = path.PyFlightPath(system, r, v, 0)

    period0 = 374942509.78053558
    performance = path.PyPerformanceData(3000, 20000)  # ve, thrust
    burn_start_t = period0 / 2

    maneuver = path.PyManeuver(
        path.PyManeuver.Types.Prograde,
        2000,
        performance,
        150,
        burn_start_t
    )
    path_.add(maneuver)

    # Get Data to plot

    t_pts = []
    v_pts = []
    apo_pts = []
    a_pts = []

    t0 = maneuver.t0 - 3
    tf = maneuver.t1 + 3
    plot_duration = tf - t0
    for i in range(N_POINTS):
        t = plot_duration / N_POINTS * i + t0

        t_pts.append(t - maneuver.t0)  # Time relative to burn t0

        orbit_data = path_.predict_orbit(t)
        v_pts.append(orbit_data.orbit.velocity.len)
        apo_pts.append(orbit_data.orbit.apoapsis)
        a_pts.append(orbit_data.orbit.semi_major_axis)

    # Plot

    gs = gridspec.GridSpec(2, 2)
    plt.subplots_adjust(hspace=0.5, wspace=0.35)

    # Velocity plot
    plt.subplot(gs[0, 0])
    plt.title('Velocity')
    plt.plot(t_pts, v_pts, 'r-')

    # Apoapsis plot
    plt.subplot(gs[1, 0])
    plt.title('Apoapsis')
    plt.plot(t_pts, apo_pts)

    # Semi major axis plot
    plt.subplot(gs[0, 1])
    plt.title('Semi Major Axis')
    plt.plot(t_pts, a_pts)

    plt.show()


@expose
def man():
    options_str = '\t' + '\n\t'.join(sorted(exposed_functions))
    print(f'Options:\n{options_str}')


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('Expected name of function to run.')
        man()
    elif sys.argv[1] in exposed_functions:
        exposed_functions[sys.argv[1]]()
    else:
        print(f'No method with name: {sys.argv[1]}.')
        man()
