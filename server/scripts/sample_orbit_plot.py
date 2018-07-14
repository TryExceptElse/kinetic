"""
Script prototyping orbit prediction
"""
import os
import sys

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

root_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
if root_path not in sys.path:
    sys.path.append(root_path)

N_POINTS = 1000


def main() -> None:
    import server.model.const as const
    import server.model.path as path
    from server.model.system import PySystem
    from server.model.body import PyBody
    from server.model.vector import PyVector

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

    gs = gridspec.GridSpec(2, 2)
    plt.figure(1)
    plt.subplots_adjust(hspace=0.5, wspace=0.35)
    plt.plot(x_pts, y_pts, 'r-')  # line
    plt.plot(burn_start_pos.x, burn_start_pos.y, 'ro')  # current pos
    plt.axis('equal')
    plt.show()


if __name__ == '__main__':
    main()
