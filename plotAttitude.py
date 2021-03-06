import pythonUtils
from scipy.spatial.transform import Rotation as R
#-------------------------------------------------------------------

dataHeader = "ATT"
port = '/dev/ttyUSB0'
baudRate = 115200

origin = [0, 0, 0]
l = 1
colors = ['tomato', 'mediumspringgreen', 'royalblue']
axisLimits = [-1.2, 1.2]
plotNEU = True
figSize = (7, 7)

#-------------------------------------------------------------------

sp = pythonUtils.serial.Serial(port, baudrate = baudRate,
                    timeout = None, xonxoff = True, rtscts = True, dsrdtr = True)
v = [[l, 0, 0],
    [0, l, 0],
    [0, 0, l]]
axis = [0, 0, 0]

plt, fig, ax = pythonUtils.plotAxis3D(figSize, axisLimits)

def vector2Line(vector, origin):
    return [[origin_i, origin_i + vector_i] for origin_i, vector_i in zip(origin, vector)]

for i, v_i in enumerate(v):
    axis[i] = plt.plot(*vector2Line(v_i, origin), c = colors[i], marker = 'o')

if plotNEU:
    for i, v_i in enumerate(v):
        plt.plot(*vector2Line(v_i, origin), c = colors[i],
                alpha = 0.7, ls = '--', lw = '1.3')

while True:
    att = pythonUtils.getData(sp, dataHeader = dataHeader)
    if att is not None:
        rotation = R.from_euler('xyz', att, degrees = True)
        for i, v_i in enumerate(v):
            v_i = rotation.apply(v_i)
            axis[i][0].set_data_3d(*vector2Line(v_i, origin))
        fig.canvas.draw()
        fig.canvas.flush_events()